#include <QtCore/QCoreApplication>
#include <iostream>
#include "crash_handler.h"
#include "bazinama.h"

#include "DoctorDumpSDK/include/CrashRpt.h"
using namespace std;

// crash_rpt::CrashRpt g_crashRpt(
// 	"5688411d-6edf-4b90-8eab-bcc29ba9b535", // GUID assigned to this application.
// 	L"consoleBreakTest",                  // Application name that will be used in message box.
// 	L"Yara IT"                        // Company name that will be used in message box.
// );

void bazinam()
{
	bazinama b;
	b.testCrash();
}
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	// We put the dumps in the user's home directory for this example:
	Breakpad::CrashHandler::instance()->Init("./");

	cout << "salam\n";

	bazinam();

	cout << "bye\n";
	return a.exec();
}
