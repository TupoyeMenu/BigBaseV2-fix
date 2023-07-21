#pragma once
#include "common.hpp"
#include "file_manager.hpp"

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <string>

namespace big
{
	template<typename TP>
	std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
		return system_clock::to_time_t(sctp);
	}

	enum class log_color : std::uint16_t
	{
		red       = FOREGROUND_RED,
		green     = FOREGROUND_GREEN,
		blue      = FOREGROUND_BLUE,
		intensify = FOREGROUND_INTENSITY
	};

	enum LOG_FLAGS
	{
		FLAG_NO_DISK    = (1 << 0),
		FLAG_NO_CONSOLE = (1 << 1)
	};

	static const int kEventValue               = 400;
	static const int kRawValue                 = 600;
	inline constexpr auto max_padding_length   = 26;
	inline constexpr auto level_padding_length = 8;

	const LEVELS INFO_TO_FILE{INFO.value | FLAG_NO_CONSOLE, {"INFO"}}, HACKER{INFO.value, {"HACKER"}}, EVENT{kEventValue | FLAG_NO_CONSOLE, {"EVENT"}}, RAW_GREEN_TO_CONSOLE{kRawValue | FLAG_NO_DISK, {"RAW_GREEN_TO_CONSOLE"}}, RAW_RED{kRawValue, {"RAW_RED"}};

	inline log_color operator|(log_color a, log_color b)
	{
		return static_cast<log_color>(static_cast<std::underlying_type_t<log_color>>(a) | static_cast<std::underlying_type_t<log_color>>(b));
	}

	class logger;
	inline logger* g_logger{};

	class logger
	{
	public:
		explicit logger(std::string_view console_title, file log_file, bool attach_console = true) :
		    m_worker(g3::LogWorker::createLogWorker()),
		    m_attach_console(attach_console),
		    m_console_title(console_title),
		    m_file(log_file)
		{
			if (m_attach_console)
			{
				if ((m_did_console_exist = AttachConsole(GetCurrentProcessId())) == false)
					AllocConsole();

				if ((m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE)) != nullptr)
				{
					SetConsoleTitleA(m_console_title.data());
					SetConsoleOutputCP(CP_UTF8);

					m_console_out.open("CONOUT$", std::ios_base::out | std::ios_base::app);
				}
			}

			auto m_backup_path = g_file_manager.get_project_folder("./backup");
			try
			{
				if (m_file.exists())
				{
					auto file_time  = std::filesystem::last_write_time(m_file.get_path());
					auto timet      = to_time_t(file_time);
					auto local_time = std::localtime(&timet);

					m_file.move(fmt::format("./backup/{:0>2}-{:0>2}-{}-{:0>2}-{:0>2}-{:0>2}_{}",
					    local_time->tm_mon + 1,
					    local_time->tm_mday,
					    local_time->tm_year + 1900,
					    local_time->tm_hour,
					    local_time->tm_min,
					    local_time->tm_sec,
					    m_file.get_path().filename().string()));
				}
				m_file_out.open(m_file.get_path(), std::ios_base::out | std::ios_base::trunc);

				m_worker->addSink(std::make_unique<log_sink>(), &log_sink::callback);
				g3::initializeLogging(m_worker.get());
			}
			catch (std::filesystem::filesystem_error const& error)
			{
				if (m_attach_console)
					m_console_out << error.what();
			}

			g_logger = this;
		}

		~logger()
		{
			m_worker->removeAllSinks();
			m_worker.reset();
			if (!m_did_console_exist && m_attach_console)
				FreeConsole();

			g_logger = nullptr;
		}

		struct log_sink
		{
			std::map<std::string, log_color> log_colors = {{INFO.text, log_color::blue | log_color::intensify},
			    {WARNING.text, log_color::red},
			    {HACKER.text, log_color::green | log_color::intensify},
			    {FATAL.text, log_color::red | log_color::intensify},
			    {G3LOG_DEBUG.text, log_color::blue},
			    {RAW_RED.text, log_color::red},
			    {RAW_GREEN_TO_CONSOLE.text, log_color::green | log_color::intensify}};

			void callback(g3::LogMessageMover log)
			{
				g3::LogMessage log_message = log.get();
				int level_value            = log_message._level.value;

				bool is_raw = level_value == RAW_RED.value || level_value == RAW_GREEN_TO_CONSOLE.value;

				if (!(level_value & FLAG_NO_CONSOLE) && g_logger->m_attach_console)
				{
					SetConsoleTextAttribute(g_logger->m_console_handle,
					    static_cast<std::uint16_t>(log_colors[log_message._level.text]));
					g_logger->m_console_out << log_message.toString(is_raw ? format_raw : format_console) << std::flush;
				}

				if (!(level_value & FLAG_NO_DISK))
				{
					g_logger->m_file_out << log_message.toString(is_raw ? format_raw : format_file) << std::flush;
				}
			}

			static std::string format_file(const g3::LogMessage& msg)
			{
				std::string file_name_with_line = "[" + msg.file() + ":" + msg.line() + "]";
				std::stringstream out;
				out << "[" << msg.timestamp("%H:%M:%S") << "] [" << std::left << std::setw(level_padding_length)
				    << msg.level().append("]") << std::setw(max_padding_length) << file_name_with_line;
				return out.str();
			}

			static std::string format_console(const g3::LogMessage& msg)
			{
				std::stringstream out;
				out << "[" << msg.timestamp("%H:%M:%S") << "] ";
				return out.str();
			}

			static std::string format_raw(const g3::LogMessage& msg)
			{
				return "";
			}
		};

		template<typename... Args>
		inline void log_now(std::string_view format, Args const&... args)
		{
			auto message = fmt::format(format, args...);
			if (m_file_out)
				m_file_out << message << std::endl << std::flush;
		}

	private:
		bool m_attach_console;
		bool m_did_console_exist;

		std::string_view m_console_title;
		HANDLE m_console_handle;
		std::ofstream m_console_out;
		file m_file;
		std::ofstream m_file_out;
		std::unique_ptr<g3::LogWorker> m_worker;
	};


#define LOG_NOW(format, ...) g_logger->log_now(format, __VA_ARGS__)
#define HEX_TO_UPPER(value) "0x" << std::hex << std::uppercase << (DWORD64)value << std::dec << std::nouppercase
}
