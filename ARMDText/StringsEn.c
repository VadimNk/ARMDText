#include <tchar.h>
#include "StringsEn.h"

TCHAR* common_strings_en[] = {
    _T(""),
    _T("ENABLE"),
    _T("DISABLE"),
    _T("File identifier"),
    _T("Version of ARMD data structures"),
    _T("File size"),
    _T("byte(s)"),
    _T("Previous file"),
    _T("No previous file"),
    _T("Next file"),
    _T("No next file."),
    _T("File chain has been torn"),
    _T("File chain is intact."),
    _T("Length of software version string"),
    _T("Software version"),
    _T("CNC machine name"),
    _T("CNC timer tick"),
    _T("ms"),
    _T("ARMD data writting interval"),
    _T("sec"),
    _T("File creation date"),
    _T("File has been created on CNC local drive"),
    _T("File has been created on target device."),
    _T("Number of all processes"),
    _T("Process number"),
    _T("Number of all events"),
    _T("Index"),
    _T("Event"),
    _T("Account type"),
    _T("Account parameter"),
    _T("units"),
    _T("PROCESS"),
    //----------------------------------//
        _T("NOT INITIALIZED"),
        _T("INITIALIZED"),
        _T("CHARACTERIZATION FILES"),
        _T("MESSAGE"),
        _T("SUBROUTINE"),
        _T("NAME"),
        _T("PATH"),
        _T("SET"),
        _T("RESET"),
        //----------------------------------//
        _T("NO EVENT"),
        _T("SYSTEM START"),
        _T("NEW DATE"),
        _T("WORK MODE"),
        _T("FEED"),
        _T("SPINDLE SPEED"),
        _T("SYSTEM STATE"),
        _T("PROGRAM ERROR MESSAGE"),
        _T("ROUTINE"),
        _T("JOG SWITCH"),
        _T("FEED SWITCH"),
        _T("SPINDLE SWITCH"),
        _T("BLOCK NUMBER"),
        _T("TOOL NUMBER"),
        _T("CORRECTOR NUMBER"),
        _T("UAS"),
        _T("UVR"),
        _T("URL"),
        _T("COMU"),
        _T("CEFA"),
        _T("MUSP"),
        _T("REAZ"),
        _T("MACHINE IDLE TIME"),
        _T("PLC alarm"),
        _T("PLC message"),
        _T("COMMNAD FROM PROCESS"),
        _T("BLOCK FROM PROCESS"),
        _T("COMMAND LINE"),
        _T("Part Finished"),
        _T("G functions"),
        _T("RISP"),
        _T("CONP"),
        _T("SPEPNREQ"),
        _T("ASPEPN"),
        _T("WNCMT"),
        _T("WPRT"),
        _T("WPROG"),
        _T("WIZKD"),
        _T("TIME SYNCH"),
        _T("SPINDLE POWER"),
        _T("SERVICE MSG CODE"),
        //------------------------------//
        _T("file size have read from header"),
        _T("initial values"),
        _T("Header loaded"),
        _T("Can't load header"),
        _T("Data check error"),
        _T("Data is corrupted."),
        _T("Fail to read ARMD file"),
        _T("Can't set start file position"),
        _T("Can't open ARMD file"),
        _T("Retry"),
        _T("Cant't get console handle"),
        _T("Can't switch console output or input"),
        _T("Error"),
        _T("No data about events"),
        _T("No data about processes"),
        _T("No software version"),
        _T("Can't convert multi-byte to wide character"),
        _T("Inaccessible or wrong data in file"),
        _T("Can't open"),
        _T("Memory allocation error!"),
        _T("Wrong number of arguments"),
        _T("Can't get standard input handle"),
        _T("Can't get console mode"),
        _T("Can't read console input")
};

TCHAR** GetCommonStringsEn()
{
    return common_strings_en;
}