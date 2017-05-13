#include "crash_handler.h"
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#include <QString>
#include <QDebug>

#if defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#include "client/windows/sender/crash_report_sender.h"
#endif

namespace Breakpad {
    /************************************************************************/
    /* CrashHandlerPrivate                                                  */
    /************************************************************************/
    class CrashHandlerPrivate
    {
    public:
        CrashHandlerPrivate()
        {
            pHandler = NULL;
        }

        ~CrashHandlerPrivate()
        {
            delete pHandler;
        }

        void InitCrashHandler(const QString& dumpPath);
        static google_breakpad::ExceptionHandler* pHandler;
        static bool bReportCrashesToSystem;
    };

    google_breakpad::ExceptionHandler* CrashHandlerPrivate::pHandler = NULL;
    bool CrashHandlerPrivate::bReportCrashesToSystem = false;

    /************************************************************************/
    /* DumpCallback                                                         */
    /************************************************************************/
#if defined(Q_OS_WIN32)
    bool DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success)
#elif defined(Q_OS_LINUX)
    bool DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool success)
#endif
    {
        Q_UNUSED(context);
#if defined(Q_OS_WIN32)
        Q_UNUSED(_dump_dir);
        Q_UNUSED(_minidump_id);
        Q_UNUSED(assertion);
        Q_UNUSED(exinfo);
#endif
        qDebug("BreakpadQt crash");
		qDebug() << context << _dump_dir << _minidump_id << assertion << exinfo;

        /*
        NO STACK USE, NO HEAP USE THERE !!!
        Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
        */
		//////////////////// Send dump file to server///////////////////////
		// Instantiate CrashReportSender, specifying checkpoint file, where it stores information about number of dumps sent per day, to limit itself
		google_breakpad::CrashReportSender sender(L"crash.checkpoint");
		// Set maximum number of reports per day from one user, or -1 for unlimited reports
 		sender.set_max_reports_per_day(-1);
		// You can fill this map with additional params, for example, video adapter information or application's inner version, or something
 		std::map<std::wstring, std::wstring> params;
 		// Callback parameters do not give you an exact name of generated minidump file, but you can reconstruct it
 		std::map<std::wstring, std::wstring> files;
 		std::wstring filename = _dump_dir;
 		filename += L"\\";
 		filename += _minidump_id;
 		filename += L".dmp";
 		files[filename] = filename;
 		// Finally, send a report
 		google_breakpad::ReportResult r = sender.SendCrashReport(L"http://185.129.169.74:4000/crash_dumps", params, files, 0);
 
		//QString qfileName = QString::fromStdWString(filename);
		qDebug() << "file name:" << filename.c_str();
 		// Possibly notify user about success/failure
		switch (r)
		{
		case google_breakpad::RESULT_SUCCEEDED:
			qDebug() << "The server accepted the crash report.";
			break;
		case google_breakpad::RESULT_FAILED:
			qDebug() << "Failed to communicate with the server; try later";
			break;
		case google_breakpad::RESULT_REJECTED:
			qDebug() << "Successfully sent the crash report, but the server rejected it; don't resend this report";
			break;
		case google_breakpad::RESULT_THROTTLED:
			qDebug() << " No attempt was made to send the crash report, because we exceeded the maximum reports per day.";
			break;
		default:
			break;
		}
		/////////////////// End of Sending dump file to Server/////////////
        return CrashHandlerPrivate::bReportCrashesToSystem ? success : true;
    }

    void CrashHandlerPrivate::InitCrashHandler(const QString& dumpPath)
    {
        if ( pHandler != NULL )
            return;

#if defined(Q_OS_WIN32)
        std::wstring pathAsStr = (const wchar_t*)dumpPath.utf16();
//         pHandler = new google_breakpad::ExceptionHandler(
//             pathAsStr,
//             /*FilterCallback*/ 0,
//             DumpCallback,
//             /*context*/
//             0,
//             true
//             );
		pHandler = new google_breakpad::ExceptionHandler(
			pathAsStr,
			/*FilterCallback*/ NULL,
			DumpCallback,
			/*context*/ NULL,
			google_breakpad::ExceptionHandler::HANDLER_ALL,
			_MINIDUMP_TYPE::MiniDumpNormal,
			L"nullpipe",
			NULL);
#elif defined(Q_OS_LINUX)
        std::string pathAsStr = dumpPath.toStdString();
        google_breakpad::MinidumpDescriptor md(pathAsStr);
        pHandler = new google_breakpad::ExceptionHandler(
            md,
            /*FilterCallback*/ 0,
            DumpCallback,
            /*context*/ 0,
            true,
            -1
            );
#endif
    }

    /************************************************************************/
    /* CrashHandler                                                         */
    /************************************************************************/
    CrashHandler* CrashHandler::instance()
    {
        static CrashHandler globalHandler;
        return &globalHandler;
    }

    CrashHandler::CrashHandler()
    {
        d = new CrashHandlerPrivate();
    }

    CrashHandler::~CrashHandler()
    {
        delete d;
    }

    void CrashHandler::setReportCrashesToSystem(bool report)
    {
        d->bReportCrashesToSystem = report;
    }

    bool CrashHandler::writeMinidump()
    {
        bool res = d->pHandler->WriteMinidump();
        if (res) {
            qDebug("BreakpadQt: writeMinidump() success.");
        } else {
            qWarning("BreakpadQt: writeMinidump() failed.");
        }
        return res;
    }

    void CrashHandler::Init( const QString& reportPath )
    {
        d->InitCrashHandler(reportPath);
    }
}
