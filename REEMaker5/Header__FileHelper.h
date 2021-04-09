#pragma once
/// <summary>
/// Class d'aide de traduction de fichier
/// Par défaut les sorties string et wstring sont en Ansi é ==> é
/// Utilisé les versions terminant par _utf8 pour les avoir en UTF8 é ==> Ã©
/// </summary>

class FileHelper
{
public:
    FileHelper();
    FileHelper(std::wstring);
    FileHelper(std::string);
    void Initialise(std::wstring);
    std::wstring RetourneNomFichier();
    std::string RetourneNomFichierS();
    std::wstring RetourneExtension();
    std::string RetourneExtensionS();
    std::wstring RetourneFichierEtExtension();
    std::string RetourneFichierEtExtensionS();
    std::wstring RetourneDossier(bool);
    std::string RetourneDossierS(bool);
    std::wstring RetourneDossierLPATH(bool);
    std::string RetourneDossierLPATHS(bool);
    std::wstring RetourneCheminCompletLong();
    std::string RetourneCheminCompletLongS();
    std::wstring RetourneCheminCompletShort();
    std::string RetourneCheminCompletShortS();
    std::wstring RetourneCheminComplet();
    std::string RetourneCheminCompletS();

    std::wstring RetourneNomFichier_utf8();
    std::string RetourneNomFichierS_utf8();
    std::wstring RetourneExtension_utf8();
    std::string RetourneExtensionS_utf8();
    std::wstring RetourneFichierEtExtension_utf8();
    std::string RetourneFichierEtExtensionS_utf8();
    std::wstring RetourneDossier_utf8(bool);
    std::string RetourneDossierS_utf8(bool);
    std::wstring RetourneDossierLPATH_utf8(bool);
    std::string RetourneDossierLPATHS_utf8(bool);
    std::wstring RetourneCheminCompletLong_utf8();
    std::string RetourneCheminCompletLongS_utf8();
    std::wstring RetourneCheminCompletShort_utf8();
    std::string RetourneCheminCompletShortS_utf8();
    std::wstring RetourneCheminComplet_utf8();
    std::string RetourneCheminCompletS_utf8();
    std::wstring ToLengthExtendedPathName(const std::string& path);
    std::wstring ToLengthExtendedPathName(const std::wstring& path);
    std::string ToLengthExtendedPathNameS(const std::string& path);
    std::string ToLengthExtendedPathNameS(const std::wstring& path);
    std::wstring ToLengthExtendedPathName_utf8(const std::string& path);
    std::wstring ToLengthExtendedPathName_utf8(const std::wstring& path);
    std::string ToLengthExtendedPathNameS_utf8(const std::string& path);
    std::string ToLengthExtendedPathNameS_utf8(const std::wstring& path);
    std::string ConvertWideToANSI(const std::wstring& wstr);
    std::wstring ConvertAnsiToWide(const std::string& str);
    std::string ConvertWideToUtf8(const std::wstring& wstr);
    std::wstring ConvertUtf8ToWide(const std::string& str);
    bool JeSuisUnDossier();
    bool JeSuisUnFichier();
    ~FileHelper();

private:
    bool mEstDossier = false;

    bool isFile(LPCWSTR path);
    bool isFolder(LPCWSTR path);
    std::wstring mNomFichier = L"";
    std::wstring mNomFichierLPATH = L"";
    std::string mNomFichierS = "";
    std::string mNomFichierLPATHS = "";
    static constexpr char DosVolumeDelimiter{ ':' };
    static constexpr char DosDirectoryDelimiter{ '\\' };
    static constexpr char DirectoryDelimiter{ '\\' };

    static constexpr char UnixDirectoryDelimiter{ '/' };
    static constexpr char AltDirectoryDelimiter{ UnixDirectoryDelimiter };

    static bool IsDosDriveLetter(int ch);
    static bool IsDosVolumeDelimiter(int ch);
    static bool IsDirectoryDelimiter(int ch);
};

FileHelper::FileHelper(std::wstring Fichier)
{
    mNomFichier = Fichier;
    mNomFichierLPATH = ToLengthExtendedPathName(Fichier);
    mEstDossier = isFolder(mNomFichierLPATH.c_str());
    mNomFichierS = ConvertWideToANSI(mNomFichier);
    mNomFichierLPATHS = ConvertWideToANSI(mNomFichierLPATH);
}
FileHelper::FileHelper(std::string Fichier)
{
    mNomFichier = ConvertAnsiToWide(Fichier);
    mNomFichierLPATH = ToLengthExtendedPathName(Fichier);
    mEstDossier = isFolder(mNomFichierLPATH.c_str());
    mNomFichierS = ConvertWideToANSI(mNomFichier);
    mNomFichierLPATHS = ConvertWideToANSI(mNomFichierLPATH);
}
FileHelper::FileHelper()
{

}
void FileHelper::Initialise(std::wstring Fichier)
{
    mNomFichier = Fichier;
    mNomFichierLPATH = ToLengthExtendedPathName(Fichier);
    mEstDossier = isFolder(mNomFichierLPATH.c_str());
    mNomFichierS = ConvertWideToANSI(mNomFichier);
    mNomFichierLPATHS = ConvertWideToANSI(mNomFichierLPATH);
}
std::wstring FileHelper::RetourneNomFichier()
{
    if (mEstDossier)
        return L"";
    else
    {
        size_t posSLASH = mNomFichier.find_last_of(L"\\");
        if (posSLASH == std::wstring::npos)
            return L"";
        std::wstring NFIC = mNomFichier.substr(posSLASH + 1);
        NFIC = NFIC.substr(0, NFIC.length() - RetourneExtension().length());
        return (NFIC.substr(NFIC.length() - 1) == L".") ? NFIC.substr(0, NFIC.length() - 1) : NFIC;
    }
}
std::string FileHelper::RetourneNomFichierS()
{
    if (mEstDossier)
        return "";
    else
    {
        size_t posSLASH = mNomFichierS.find_last_of("\\");
        if (posSLASH == std::string::npos)
            return "";
        std::string NFIC = mNomFichierS.substr(posSLASH + 1);
        NFIC = NFIC.substr(0, NFIC.length() - RetourneExtensionS().length());
        return (NFIC.substr(NFIC.length() - 1) == ".") ? NFIC.substr(0, NFIC.length() - 1) : NFIC;
    }
}
std::wstring FileHelper::RetourneExtension()
{
    if (mEstDossier)
        return L"";
    else
    {
        size_t posSLASH = mNomFichier.find_last_of(L"\\");
        if (posSLASH == std::wstring::npos)
            return L"";
        std::wstring lANA = mNomFichier.substr(posSLASH + 1);
        size_t posDOT = lANA.find_last_of(L".");
        if (posDOT == std::wstring::npos)
            return L"";
        return lANA.substr(posDOT + 1);
    }
}
std::string FileHelper::RetourneExtensionS()
{
    if (mEstDossier)
        return "";
    else
    {
        size_t posSLASH = mNomFichierS.find_last_of("\\");
        if (posSLASH == std::string::npos)
            return "";
        std::string lANA = mNomFichierS.substr(posSLASH + 1);
        size_t posDOT = lANA.find_last_of(".");
        if (posDOT == std::string::npos)
            return "";
        return lANA.substr(posDOT + 1);
    }
}

std::wstring FileHelper::RetourneFichierEtExtension()
{
    if (mEstDossier)
        return L"";
    else
    {
        size_t posSLASH = mNomFichier.find_last_of(L"\\");
        if (posSLASH == std::wstring::npos)
            return L"";
        return mNomFichier.substr(posSLASH + 1);
    }
}
std::string FileHelper::RetourneFichierEtExtensionS()
{
    if (mEstDossier)
        return "";
    else
    {
        size_t posSLASH = mNomFichierS.find_last_of("\\");
        if (posSLASH == std::string::npos)
            return "";
        return mNomFichierS.substr(posSLASH + 1);
    }
}

std::wstring FileHelper::RetourneDossier(bool TermineParSlash)
{
    if (mEstDossier)
    {
        if (mNomFichier.substr(mNomFichier.length() - 1) == L"\\")
            return mNomFichier.substr(0, mNomFichier.length() + (TermineParSlash ? 0 : -1));//Termine par un Slash
        else
            return mNomFichier + (TermineParSlash ? L"\\" : L"");//Termine pas par un Slash
    }
    else
    {   //C'est un fichier
        size_t posSLASH = mNomFichier.find_last_of(L"\\");
        if (posSLASH == std::wstring::npos)
            return L"";
        return mNomFichier.substr(0, posSLASH + (TermineParSlash ? 1 : 0));
    }
}
std::string FileHelper::RetourneDossierS(bool TermineParSlash)
{
    if (mEstDossier)
    {
        if (mNomFichierS.substr(mNomFichierS.length() - 1) == "\\")
            return mNomFichierS.substr(0, mNomFichierS.length() + (TermineParSlash ? 0 : -1));//Termine par un Slash
        else
            return mNomFichierS + (TermineParSlash ? "\\" : "");//Termine pas par un Slash
    }
    else
    {   //C'est un fichier
        size_t posSLASH = mNomFichierS.find_last_of("\\");
        if (posSLASH == std::string::npos)
            return "";
        return mNomFichierS.substr(0, posSLASH + (TermineParSlash ? 1 : 0));
    }
}
std::string FileHelper::RetourneDossierLPATHS(bool TermineParSlash)
{
    std::string lPath = RetourneDossierS(TermineParSlash);
    return ToLengthExtendedPathNameS(lPath);
}
std::wstring FileHelper::RetourneDossierLPATH(bool TermineParSlash)
{
    std::wstring lPath = RetourneDossier(TermineParSlash);
    return ToLengthExtendedPathName(lPath);
}
std::string FileHelper::RetourneCheminCompletLongS()
{
    return mNomFichierLPATHS;
}
std::wstring FileHelper::RetourneCheminCompletLong()
{
    return mNomFichierLPATH;
}
std::wstring FileHelper::RetourneCheminCompletShort()
{
    constexpr int BUFSIZE = 4096;
    DWORD  retval = 0;
    wchar_t  buf[BUFSIZE] = TEXT("");
    if (mNomFichierLPATH != L"")
        retval = GetShortPathNameW(std::wstring(mNomFichierLPATH + L"\0").c_str(), buf, BUFSIZE);
    return std::wstring(buf);
}
std::string FileHelper::RetourneCheminCompletShortS()
{
    constexpr int BUFSIZE = 4096;
    DWORD  retval = 0;
    char  buf[BUFSIZE] = "";
    if (mNomFichierLPATHS != "")
        retval = GetShortPathNameA(std::string(mNomFichierLPATHS + "\0").c_str(), buf, BUFSIZE);
    return std::string(buf);
}
std::wstring FileHelper::RetourneCheminComplet()
{
    return mNomFichier;
}
std::string FileHelper::RetourneCheminCompletS()
{
    return mNomFichierS;
}



std::wstring FileHelper::RetourneNomFichier_utf8()
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneNomFichier()));
}
std::string FileHelper::RetourneNomFichierS_utf8()
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneNomFichierS()));
}
std::wstring FileHelper::RetourneExtension_utf8()
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneExtension()));
}
std::string FileHelper::RetourneExtensionS_utf8()
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneExtensionS()));
}
std::wstring FileHelper::RetourneFichierEtExtension_utf8()
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneFichierEtExtension()));
}
std::string FileHelper::RetourneFichierEtExtensionS_utf8()
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneFichierEtExtensionS()));
}
std::wstring FileHelper::RetourneDossier_utf8(bool TermineParSlash)
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneDossier(TermineParSlash)));
}
std::string FileHelper::RetourneDossierS_utf8(bool TermineParSlash)
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneDossierS(TermineParSlash)));
}
std::string FileHelper::RetourneDossierLPATHS_utf8(bool TermineParSlash)
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneDossierLPATHS(TermineParSlash)));
}
std::wstring FileHelper::RetourneDossierLPATH_utf8(bool TermineParSlash)
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneDossierLPATH(TermineParSlash)));
}
std::string FileHelper::RetourneCheminCompletLongS_utf8()
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneCheminCompletLongS()));
}
std::wstring FileHelper::RetourneCheminCompletLong_utf8()
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneCheminCompletLong()));
}
std::wstring FileHelper::RetourneCheminCompletShort_utf8()
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneCheminCompletShort()));
}
std::string FileHelper::RetourneCheminCompletShortS_utf8()
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneCheminCompletShortS()));
}
std::wstring FileHelper::RetourneCheminComplet_utf8()
{
    return ConvertAnsiToWide(ConvertWideToUtf8(RetourneCheminComplet()));
}
std::string FileHelper::RetourneCheminCompletS_utf8()
{
    return ConvertWideToUtf8(ConvertAnsiToWide(RetourneCheminCompletS()));
}
std::string FileHelper::ConvertWideToANSI(const std::wstring& wstr)
{
    int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}
std::wstring FileHelper::ConvertAnsiToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}
std::string FileHelper::ConvertWideToUtf8(const std::wstring& wstr)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}
std::wstring FileHelper::ConvertUtf8ToWide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}
bool FileHelper::isFile(LPCWSTR path) {
    auto pthSTAT = GetFileAttributesW(path);
    if (pthSTAT == INVALID_FILE_ATTRIBUTES) return false;
    if (pthSTAT & FILE_ATTRIBUTE_DIRECTORY) return false;
    return true;
}
bool FileHelper::isFolder(LPCWSTR path) {
    auto pthSTAT = GetFileAttributesW(path);
    if (pthSTAT == INVALID_FILE_ATTRIBUTES) return false;
    if (pthSTAT & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}
bool FileHelper::JeSuisUnDossier()
{
    return isFolder(mNomFichierLPATH.c_str());
}
bool FileHelper::JeSuisUnFichier()
{
    return isFile(mNomFichierLPATH.c_str());
}

bool FileHelper::IsDosDriveLetter(int ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}
bool FileHelper::IsDosVolumeDelimiter(int ch)
{
    return ch == DosVolumeDelimiter;
}
bool FileHelper::IsDirectoryDelimiter(int ch)
{
    if (ch == DirectoryDelimiter)
        return true;
    return ch == AltDirectoryDelimiter;
}

std::wstring FileHelper::ToLengthExtendedPathName(const std::string& path)
{
    if (path.empty())
        return L"\\\\?\\";
    std::wstring wpath = ConvertUtf8ToWide(path);
    DWORD len = GetFullPathNameW(wpath.c_str(), 0, nullptr, nullptr);
    if (len == 0)
        return L"";// Erreur GetFullPathNameW
    std::vector<wchar_t> absPath;
    absPath.resize(len);
    len = GetFullPathNameW(wpath.c_str(), len, &absPath[0], nullptr);
    if (len == 0 || len >= absPath.max_size())
        return L"";// Erreur GetFullPathNameW
    if (IsDosDriveLetter(absPath[0]) && IsDosVolumeDelimiter(absPath[1]) && IsDirectoryDelimiter(absPath[2]))
        return L"\\\\?\\" + std::wstring(absPath.begin(), absPath.end());
    else if (IsDirectoryDelimiter(absPath[0]) && IsDirectoryDelimiter(absPath[1]))
        if (absPath[2] == L'?' && IsDirectoryDelimiter(absPath[3]))
            if (absPath[4] == L'U' && absPath[5] == L'N' && absPath[6] == L'C' && IsDirectoryDelimiter(absPath[7]))
                return std::wstring(absPath.begin(), absPath.end());
            else if (IsDosDriveLetter(absPath[4]) && IsDirectoryDelimiter(absPath[5]))
                return std::wstring(absPath.begin(), absPath.end());
            else if (absPath.size() > 4)
                return std::wstring(absPath.begin(), absPath.end());
        else if (absPath[2] == '.' && IsDirectoryDelimiter(absPath[3]))
            if (IsDosDriveLetter(absPath[4]) && IsDosVolumeDelimiter(absPath[5]))
                return L"\\\\?\\" + std::wstring(&absPath[4]);
            else if (absPath.size() > 4)
                return std::wstring(absPath.begin(), absPath.end());
        else if (absPath.size() > 2)
            return L"\\\\?\\UNC\\" + std::wstring(&absPath[2]);
    return wpath;
}
std::wstring FileHelper::ToLengthExtendedPathName(const std::wstring& path)
{
    if (path.empty())
        return L"\\\\?\\";
    std::wstring wpath = path;
    DWORD len = GetFullPathNameW(wpath.c_str(), 0, nullptr, nullptr);
    if (len == 0)
        return L"";// Erreur GetFullPathNameW
    std::vector<wchar_t> absPath;
    absPath.resize(len);
    len = GetFullPathNameW(wpath.c_str(), len, &absPath[0], nullptr);
    if (len == 0 || len >= absPath.max_size())
        return L"";// Erreur GetFullPathNameW
    if (IsDosDriveLetter(absPath[0]) && IsDosVolumeDelimiter(absPath[1]) && IsDirectoryDelimiter(absPath[2]))
        return L"\\\\?\\" + std::wstring(absPath.begin(), absPath.end());
    else if (IsDirectoryDelimiter(absPath[0]) && IsDirectoryDelimiter(absPath[1]))
    {
        if (absPath[2] == L'?' && IsDirectoryDelimiter(absPath[3]))
            if (absPath[4] == L'U' && absPath[5] == L'N' && absPath[6] == L'C' && IsDirectoryDelimiter(absPath[7]))
                return std::wstring(absPath.begin(), absPath.end());
            else if (IsDosDriveLetter(absPath[4]) && IsDirectoryDelimiter(absPath[5]))
                return std::wstring(absPath.begin(), absPath.end());
            else if (absPath.size() > 4)
                return std::wstring(absPath.begin(), absPath.end());
        else if (absPath[2] == '.' && IsDirectoryDelimiter(absPath[3]))
            if (IsDosDriveLetter(absPath[4]) && IsDosVolumeDelimiter(absPath[5]))
                return L"\\\\?\\" + std::wstring(&absPath[4]);
            else if (absPath.size() > 4)
                return std::wstring(absPath.begin(), absPath.end());
        else if (absPath.size() > 2)
            return L"\\\\?\\UNC\\" + std::wstring(&absPath[2]);
    }
    return wpath;
}

std::string FileHelper::ToLengthExtendedPathNameS(const std::string& path)
{
    return ConvertWideToANSI(ToLengthExtendedPathName(path));
}
std::string FileHelper::ToLengthExtendedPathNameS(const std::wstring& path)
{
    return ConvertWideToANSI(ToLengthExtendedPathName(path));
}

std::wstring FileHelper::ToLengthExtendedPathName_utf8(const std::string& path)
{
    return ConvertAnsiToWide(ConvertWideToUtf8(ToLengthExtendedPathName(path)));
}
std::wstring FileHelper::ToLengthExtendedPathName_utf8(const std::wstring& path)
{
    return ConvertAnsiToWide(ConvertWideToUtf8(ToLengthExtendedPathName(path)));
}
std::string FileHelper::ToLengthExtendedPathNameS_utf8(const std::string& path)
{
    return ConvertWideToUtf8(ConvertAnsiToWide(ToLengthExtendedPathNameS(path)));
}
std::string FileHelper::ToLengthExtendedPathNameS_utf8(const std::wstring& path)
{
    return ConvertWideToUtf8(ConvertAnsiToWide(ToLengthExtendedPathNameS(path)));
}



FileHelper::~FileHelper()
{
}