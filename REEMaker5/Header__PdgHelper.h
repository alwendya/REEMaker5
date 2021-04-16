/*
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
* ░▒▓     _    _      _                   _____                       _                            _		▓▒░
* ░▒▓    | |  | |    | |                 |  __ \                     | |                          | |		▓▒░
* ░▒▓    | |__| | ___| |_ __   ___ _ __  | |__) |_ _  __ _  ___    __| | ___    __ _  __ _ _ __ __| | ___	▓▒░
* ░▒▓    |  __  |/ _ \ | '_ \ / _ \ '__| |  ___/ _` |/ _` |/ _ \  / _` |/ _ \  / _` |/ _` | '__/ _` |/ _ \	▓▒░
* ░▒▓    | |  | |  __/ | |_) |  __/ |    | |  | (_| | (_| |  __/ | (_| |  __/ | (_| | (_| | | | (_| |  __/	▓▒░
* ░▒▓    |_|  |_|\___|_| .__/ \___|_|    |_|   \__,_|\__, |\___|  \__,_|\___|  \__, |\__,_|_|  \__,_|\___|	▓▒░
* ░▒▓   			   | |                            __/ |                     __/ |						▓▒░
* ░▒▓   			   |_|                           |___/                     |___/						▓▒░
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
*  REEMaker 5 __ Grégory WENTZEL (c) 2021
*/
#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>
#include <vector>
#include <podofo/podofo.h>

bool _tracePDGHELPER(const wchar_t* format, ...);
bool _tracePDGHELPER(const char* format, ...);
#define TRACE_PDG _tracePDGHELPER

#ifndef NDEBUG
#define TextDebug
#endif
class PDGHelper
{
public:
	enum class TypeCommande
	{
		DESSINELIGNE, DESSINERECTANGLEVIDE, DESSINERECTANGLEGRILLE, DESSINERECTANGLEREMPLIS, DESSINETEXTE, DESSINETEXTEMULTILIGNE, DESSINETEXTEQUESTION, DESSINETEXTEMULTILIGNEQUESTION, INSEREIMAGE, DESSINECHECKBOX, DESSINECHECKBOXQUESTION, PAGESUIVANTE, DESSINEMULTICHECKBOXQUESTION
	};

	struct Question
	{
		int IndexQuestion;
		std::string LaQuestion;
		std::string AideQuestion;
		std::vector<char> vDefautQuestion;
		bool Obligatoire = false;
		bool EstCheckbox = false;
		bool CheckboxValue = false;
		bool EstLigneTexte = false;
		bool EstMultiLigneTexte = false;
	};
	struct structReplaceArray
	{
		std::string ReferenceSite = "";
		std::string NumeroTranche = "";
		std::string ReferenceREE = "";
		std::string IndiceREE = "";
		int REErouge = 0;
		int REEvert = 0;
		int REEbleu = 0;
	};
	PDGHelper();
	bool OpenAndParseConfig_v2(std::wstring CheminConfig);
	int DrawOnPage_v2(PoDoFo::PdfPainter&, PoDoFo::PdfDocument&);
	int ItemCount();
	int ItemQuestionCount();
	std::string DocumentOuvertANSI();
	std::string DocumentOuvertUTF8();
	std::wstring DocumentOuvertWIDE();
	void SetBaseModelePath(std::wstring);
	bool BurstVersDisque(std::wstring);
	void ClearList();
	std::string StringFromVec(std::vector<char>);
	std::vector<Question> ListeQuestion;
	structReplaceArray ArrayFromREEMAKER;
	~PDGHelper();
private:
	struct CmdKeys
	{
		std::string Keys;
		std::string Valeur;
	};
	struct structCOMMANDE
	{
		TypeCommande mTypeCommande;
		std::vector<CmdKeys> mVecCommande;
	};
	std::vector<structCOMMANDE> vecCommandeList;
	static inline void ltrim(std::string& s);
	static inline void rtrim(std::string& s);
	static inline void trim(std::string& s);
	static inline std::string ltrim_copy(std::string s);
	static inline std::string rtrim_copy(std::string s);
	static inline std::string trim_copy(std::string s);
	void replaceAll(std::string& str, const std::string& from, const std::string& to);
	std::string RetourneCleStr(std::vector<CmdKeys>& lVecKey, std::string Cle);
	double RetourneCleDouble(std::vector<CmdKeys>& lVecKey, std::string Cle);
	int RetourneCleInt(std::vector<CmdKeys>& lVecKey, std::string Cle);
	bool RetourneCleBool(std::vector<CmdKeys>& lVecKey, std::string Cle);
	std::string CapTo(std::string, int);
	std::string to_utf8(const std::string);
	std::string from_utf8(const std::string);
	std::string ConvertWideToANSI(const std::wstring& wstr);
	std::wstring ConvertAnsiToWide(const std::string& str);
	std::string ConvertWideToUtf8(const std::wstring& wstr);
	std::wstring ConvertUtf8ToWide(const std::string& str);
	double GetMaxFontSize(PoDoFo::PdfFont*&, double, double, double, std::string, double = 1.1);
	std::wstring BaseModelePath;
	std::wstring DocumentOuvertWstring;
};

PDGHelper::PDGHelper()
{
}
inline std::string PDGHelper::DocumentOuvertUTF8()
{
	return ConvertWideToUtf8(DocumentOuvertWstring);
}
inline std::string PDGHelper::DocumentOuvertANSI()
{
	return ConvertWideToANSI(DocumentOuvertWstring);
}
inline std::wstring PDGHelper::DocumentOuvertWIDE()
{
	return DocumentOuvertWstring;
}
void PDGHelper::SetBaseModelePath(std::wstring lBaseModelePath)
{
	BaseModelePath = lBaseModelePath;
	if (BaseModelePath.substr(BaseModelePath.length() - 1, 1) != L"\\")
		BaseModelePath += L"\\";
}
bool PDGHelper::OpenAndParseConfig_v2(std::wstring CheminConfig)
{
	DocumentOuvertWstring = L"";
	std::ifstream FichierConfig(BaseModelePath + CheminConfig, std::wifstream::in);
	if (FichierConfig.fail())
		return false;
	DocumentOuvertWstring = CheminConfig;
	std::vector<std::string> vecFichierPDG;
	vecFichierPDG.clear();
	for (std::string line; std::getline(FichierConfig, line); )
	{
		if (line.length() > 2)
		{
			if (line.substr(0, 2) == "ï»")//C'est le BOM UTF8
				line = line.substr(3); //Suppresiion BOM
			if (line.length() < 2)
				continue;//Trop petit maintenant, n passe à l'item suivant
			if (line.substr(0, 2) == "::" || line.substr(0, 2) == "ï»")//C'est un commentaire on ignore ou le BOM du fichier UTF8
				continue;
			trim(line);
			vecFichierPDG.push_back(line);
		}
	}
	FichierConfig.close();
	vecCommandeList.clear();
	structCOMMANDE mStructCommand;
	for (size_t compteLigne = 0; compteLigne < vecFichierPDG.size(); compteLigne++)
	{
		std::string trimmedLine = vecFichierPDG[compteLigne];
		if (trimmedLine.substr(0, 2) != "--")//C'est pas un argument, on push le vec precedent puis on re-on init mVecCommande avec la fonction
		{
			if (mStructCommand.mVecCommande.size() > 0)//Plus que 0, c'est que l'on a cree un vecteur argument
				vecCommandeList.push_back(mStructCommand);//Donc on le sauve
			mStructCommand.mVecCommande.clear();//On re-init
			if (trimmedLine == "DESSINELIGNE")
				mStructCommand.mTypeCommande = TypeCommande::DESSINELIGNE;
			if (trimmedLine == "DESSINERECTANGLEVIDE")
				mStructCommand.mTypeCommande = TypeCommande::DESSINERECTANGLEVIDE;
			if (trimmedLine == "DESSINERECTANGLEGRILLE")
				mStructCommand.mTypeCommande = TypeCommande::DESSINERECTANGLEGRILLE;
			if (trimmedLine == "DESSINERECTANGLEREMPLIS")
				mStructCommand.mTypeCommande = TypeCommande::DESSINERECTANGLEREMPLIS;
			if (trimmedLine == "DESSINETEXTE")
				mStructCommand.mTypeCommande = TypeCommande::DESSINETEXTE;
			if (trimmedLine == "DESSINETEXTEMULTILIGNE")
				mStructCommand.mTypeCommande = TypeCommande::DESSINETEXTEMULTILIGNE;
			if (trimmedLine == "DESSINETEXTEQUESTION")
				mStructCommand.mTypeCommande = TypeCommande::DESSINETEXTEQUESTION;
			if (trimmedLine == "DESSINETEXTEMULTILIGNEQUESTION")
				mStructCommand.mTypeCommande = TypeCommande::DESSINETEXTEMULTILIGNEQUESTION;
			if (trimmedLine == "INSEREIMAGE")
				mStructCommand.mTypeCommande = TypeCommande::INSEREIMAGE;
			if (trimmedLine == "DESSINECHECKBOX")
				mStructCommand.mTypeCommande = TypeCommande::DESSINECHECKBOX;
			if (trimmedLine == "DESSINECHECKBOXQUESTION")
				mStructCommand.mTypeCommande = TypeCommande::DESSINECHECKBOXQUESTION;
			if (trimmedLine == "DESSINEMULTICHECKBOXQUESTION")
				mStructCommand.mTypeCommande = TypeCommande::DESSINEMULTICHECKBOXQUESTION;
			if (trimmedLine == "PAGESUIVANTE")
			{
				mStructCommand.mTypeCommande = TypeCommande::PAGESUIVANTE;
				CmdKeys mCmdKeys;
				mStructCommand.mVecCommande.push_back(mCmdKeys);//On crée le nouveau
			}
		}
		else if (trimmedLine.substr(0, 2) == "--")
		{
			CmdKeys mCmdKeys;
			if (trimmedLine.find_first_of('=') != std::string::npos)
			{//Il y a un égale c'et une variable
				mCmdKeys.Keys = trimmedLine.substr(2, trimmedLine.find_first_of('=') - 2);
				mCmdKeys.Valeur = trimmedLine.substr(mCmdKeys.Keys.length() + 2 + 1);
			}
			else
			{//Il n'y a pas d'égale c'est un flag
				mCmdKeys.Keys = trimmedLine.substr(2);
				mCmdKeys.Valeur = "true";//On le met à true, l'important c'est qu'il ait une valeur
			}
			std::transform(mCmdKeys.Keys.begin(), mCmdKeys.Keys.end(), mCmdKeys.Keys.begin(), [](unsigned char c) { return std::tolower(c); });
			mStructCommand.mVecCommande.push_back(mCmdKeys);//On crée le nouveau
		}
	}
	if (mStructCommand.mVecCommande.size() > 0)//On a fini et un arhhument est construit
		vecCommandeList.push_back(mStructCommand);//On le sauve
	ListeQuestion.clear();
	for (size_t i = 0; i < vecCommandeList.size(); i++)
	{
		if (vecCommandeList[i].mTypeCommande == TypeCommande::DESSINECHECKBOXQUESTION || vecCommandeList[i].mTypeCommande == TypeCommande::DESSINEMULTICHECKBOXQUESTION || vecCommandeList[i].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION || vecCommandeList[i].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION)
		{
			Question mQuestion;
			mQuestion.Obligatoire = RetourneCleBool(vecCommandeList[i].mVecCommande, "obligatoire");
			mQuestion.LaQuestion = RetourneCleStr(vecCommandeList[i].mVecCommande, "question");
			mQuestion.AideQuestion = RetourneCleStr(vecCommandeList[i].mVecCommande, "aidequestion");
			mQuestion.IndexQuestion = i;
			mQuestion.vDefautQuestion.clear();
			mQuestion.vDefautQuestion.resize(RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion").size());
			if (mQuestion.vDefautQuestion.size() > 0)
				memcpy_s(&mQuestion.vDefautQuestion[0], RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion").size(),
					&RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion")[0], RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion").size());
			if (vecCommandeList[i].mTypeCommande == TypeCommande::DESSINECHECKBOXQUESTION ||
				vecCommandeList[i].mTypeCommande == TypeCommande::DESSINEMULTICHECKBOXQUESTION)
			{
				mQuestion.EstCheckbox = true;
				mQuestion.CheckboxValue = (StringFromVec(mQuestion.vDefautQuestion) == "oui");
				OutputDebugString(L"");
			}
			if (vecCommandeList[i].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION ||
				vecCommandeList[i].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION)
			{
				int MaxLength = RetourneCleInt(vecCommandeList[i].mVecCommande, "max");
				if (MaxLength == INT16_MAX)
					mQuestion.vDefautQuestion.resize(4096);
				else
					mQuestion.vDefautQuestion.resize((size_t)MaxLength + 1);
			}
			if (vecCommandeList[i].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION)
				mQuestion.EstLigneTexte = true;
			if (vecCommandeList[i].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION)
				mQuestion.EstMultiLigneTexte = true;
			ListeQuestion.push_back(mQuestion);
		}
	}
	return true;
}

inline bool PDGHelper::BurstVersDisque(std::wstring FichierSortie)
{
	if (vecCommandeList.size() == 0)
	{
		TRACE_PDG(L"INFO: Aucune commande à dessiner...\n");
		return false;
	}

	std::wofstream Fichier(FichierSortie, std::wofstream::out);
	Fichier.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));//UTF8
	if (Fichier.fail())
		return false;

	for (size_t lArg = 0; lArg < vecCommandeList.size(); lArg++)
	{
		switch (vecCommandeList[lArg].mTypeCommande)
		{
		case TypeCommande::DESSINELIGNE:
			Fichier << "DESSINELIGNE\n";
			break;
		case TypeCommande::DESSINERECTANGLEVIDE:
			Fichier << "DESSINERECTANGLEVIDE\n";
			break;
		case TypeCommande::DESSINERECTANGLEGRILLE:
			Fichier << "DESSINERECTANGLEGRILLE\n";
			break;
		case TypeCommande::DESSINERECTANGLEREMPLIS:
			Fichier << "DESSINERECTANGLEREMPLIS\n";
			break;
		case TypeCommande::DESSINETEXTE:
			Fichier << "DESSINETEXTE\n";
			break;
		case TypeCommande::DESSINETEXTEMULTILIGNE:
			Fichier << "DESSINETEXTEMULTILIGNE\n";
			break;
		case TypeCommande::DESSINETEXTEQUESTION:
			Fichier << "DESSINETEXTEQUESTION\n";
			break;
		case TypeCommande::DESSINETEXTEMULTILIGNEQUESTION:
			Fichier << "DESSINETEXTEMULTILIGNEQUESTION\n";
			break;
		case TypeCommande::INSEREIMAGE:
			Fichier << "INSEREIMAGE\n";
			break;
		case TypeCommande::DESSINECHECKBOX:
			Fichier << "DESSINECHECKBOX\n";
		case TypeCommande::DESSINECHECKBOXQUESTION:
			Fichier << "DESSINECHECKBOXQUESTION\n";
			break;
		case TypeCommande::DESSINEMULTICHECKBOXQUESTION:
			Fichier << "DESSINECHECKBOXQUESTION\n";
			break;
		case TypeCommande::PAGESUIVANTE:
			Fichier << "PAGESUIVANTE\n";
			break;
		}
		for (size_t i = 0; i < vecCommandeList[lArg].mVecCommande.size(); i++)
		{
			std::wstring wKeys = ConvertUtf8ToWide(vecCommandeList[lArg].mVecCommande[i].Keys);
			std::wstring wValeur = ConvertUtf8ToWide(vecCommandeList[lArg].mVecCommande[i].Valeur);
			//Cles avec valeur (int ou double)
			if (wKeys == L"debutx" || wKeys == L"debuty" || wKeys == L"finx" || wKeys == L"finy"
				|| wKeys == L"rouge" || wKeys == L"vert" || wKeys == L"bleu"
				|| wKeys == L"epaisseur" || wKeys == L"largeur" || wKeys == L"hauteur"
				|| wKeys == L"nombrecolonne" || wKeys == L"nombreligne"
				|| wKeys == L"remplisrouge" || wKeys == L"remplisvert" || wKeys == L"remplisbleu"
				|| wKeys == L"alignlargeur" || wKeys == L"alignhauteur" || wKeys == L"taillepolice"
				|| wKeys == L"split" || wKeys == L"max" || wKeys == L"debutx1"
				|| wKeys == L"debutx2" || wKeys == L"debuty1" || wKeys == L"debuty2"
				)
				Fichier << L"--" << wKeys.c_str() << L"=" << wValeur.c_str() << std::endl;
			//Cles sans valeur ==> FLAG
			else if (wKeys == L"gras" || wKeys == L"grasitalic" || wKeys == L"italic" || wKeys == L"monospace" || wKeys == L"obligatoire")
				Fichier << L"--" << wKeys.c_str() << std::endl;
			//Cles avec string
			else if (wKeys == L"texte" || wKeys == L"question" || wKeys == L"aidequestion" || wKeys == L"chemin")
				Fichier << L"--" << wKeys.c_str() << L"=" << wValeur.c_str() << std::endl;
			else if (wKeys == L"defautquestion")
			{
				std::wstring valDefautquestion = L"";
				for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
					if (ListeQuestion[iReponse].IndexQuestion == lArg)
					{
						if (ListeQuestion[iReponse].EstCheckbox)
							valDefautquestion = ListeQuestion[iReponse].CheckboxValue ? L"oui" : L"non";
						else
							valDefautquestion = ConvertUtf8ToWide(StringFromVec(ListeQuestion[iReponse].vDefautQuestion));
						break;
					}
				Fichier << L"--" << wKeys.c_str() << L"=\"" << valDefautquestion.c_str() << L"\"" << std::endl;
			}
		}
	}

	return true;
}

inline int PDGHelper::DrawOnPage_v2(PoDoFo::PdfPainter& Painter, PoDoFo::PdfDocument& Document)
{
	int NombrePageCree = 1;
	if (vecCommandeList.size() == 0)
	{
		TRACE_PDG(L"INFO: Aucune commande à dessiner...\n");
		return 0;
	}
	char path[4096];
	DWORD length;
	length = GetModuleFileNameA(NULL, path, 4096);
	/*
	* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
	* ░▒▓   Chemin de base de l'executable, fini par \   ▓▒░
	* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
	*/
	std::string CheminBASE(path);
	CheminBASE = CheminBASE.substr(0, CheminBASE.find_last_of("\\") + 1);
	std::string CheminRobotoMonoRegular = CheminBASE + "Police\\RobotoMono-Regular.ttf";
	if (!std::filesystem::exists(CheminRobotoMonoRegular))
		return 0;
	std::string CheminRobotoMonoBold = CheminBASE + "Police\\RobotoMono-Bold.ttf";
	if (!std::filesystem::exists(CheminRobotoMonoBold))
		return 0;
	std::string CheminRobotoMonoItalic = CheminBASE + "Police\\RobotoMono-Italic.ttf";
	if (!std::filesystem::exists(CheminRobotoMonoItalic))
		return 0;
	std::string CheminRobotoMonoBoldItalic = CheminBASE + "Police\\RobotoMono-BoldItalic.ttf";
	if (!std::filesystem::exists(CheminRobotoMonoBoldItalic))
		return 0;

	std::string CheminRobotoRegular = CheminBASE + "Police\\Roboto-Regular.ttf";
	if (!std::filesystem::exists(CheminRobotoRegular))
		return 0;
	std::string CheminRobotoBold = CheminBASE + "Police\\Roboto-Bold.ttf";
	if (!std::filesystem::exists(CheminRobotoBold))
		return 0;
	std::string CheminRobotoItalic = CheminBASE + "Police\\Roboto-Italic.ttf";
	if (!std::filesystem::exists(CheminRobotoItalic))
		return 0;
	std::string CheminRobotoBoldItalic = CheminBASE + "Police\\Roboto-BoldItalic.ttf";
	if (!std::filesystem::exists(CheminRobotoBoldItalic))
		return 0;
	/*
	* 𝕆𝕣𝕚𝕘𝕚𝕟𝕖 𝕡𝕠𝕝𝕚𝕔𝕖 (𝕝𝕚𝕔𝕖𝕟𝕔𝕖 𝔸𝕡𝕒𝕔𝕙𝕖): https://github.com/google/fonts/tree/main/apache
	*/
	PoDoFo::PdfFont* pFontMono = Document.CreateFontSubset("Roboto Mono", false, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoMonoRegular.c_str());
	PoDoFo::PdfFont* pFontMonoBOLD = Document.CreateFontSubset("Roboto Mono Bold", true, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoMonoBold.c_str());
	PoDoFo::PdfFont* pFontMonoITALIC = Document.CreateFontSubset("Roboto Mono Italic", false, true, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoMonoItalic.c_str());
	PoDoFo::PdfFont* pFontMonoBOLDITALIC = Document.CreateFontSubset("Roboto Mono Bold Italic", true, true, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoMonoBoldItalic.c_str());
	PoDoFo::PdfFont* pFontReg = Document.CreateFontSubset("Roboto", false, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoRegular.c_str());
	PoDoFo::PdfFont* pFontRegBOLD = Document.CreateFontSubset("Roboto Bold", true, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoBold.c_str());
	PoDoFo::PdfFont* pFontRegITALIC = Document.CreateFontSubset("Roboto Italic", false, true, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoItalic.c_str());
	PoDoFo::PdfFont* pFontRegBOLDITALIC = Document.CreateFontSubset("Roboto Bold Italic", true, true, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminRobotoBoldItalic.c_str());
	if (pFontMono == NULL || pFontMonoBOLD == NULL || pFontMonoBOLDITALIC == NULL || pFontMonoITALIC == NULL)
	{
		TRACE_PDG(L"ERROR: Les font Monospace n'ont pu être chargées\n");
		return 0;
	}
	if (pFontReg == NULL || pFontRegBOLD == NULL || pFontRegBOLDITALIC == NULL || pFontRegITALIC == NULL)
	{
		TRACE_PDG(L"ERROR: Les font Regular n'ont pu être chargées\n");
		return 0;
	}
	double PageWidth = Painter.GetPage()->GetPageSize().GetWidth();
	double PageHeight = Painter.GetPage()->GetPageSize().GetHeight();

	for (size_t lArg = 0; lArg < vecCommandeList.size(); lArg++)
	{
		/*
		* (っ◔◡◔)っ 𝙋𝙍𝙀𝙇𝙊𝘼𝘿 𝘿𝙀𝙎 𝙄𝙉𝙏𝙎
		*/
#pragma region Region_Int
		int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
		if (ValRouge == -1)
			ValRouge = ArrayFromREEMAKER.REErouge;
		if (ValRouge == INT16_MAX)
			ValRouge = 0;
		int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
		if (valVert == -1)
			valVert = ArrayFromREEMAKER.REEvert;
		if (valVert == INT16_MAX)
			valVert = 0;
		int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
		if (valBleu == -1)
			valBleu = ArrayFromREEMAKER.REEbleu;
		if (valBleu == INT16_MAX)
			valBleu = 0;
		int ValRemplisRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisrouge");
		if (ValRemplisRouge == -1)
			ValRemplisRouge = ArrayFromREEMAKER.REErouge;
		if (ValRemplisRouge == INT16_MAX)
			ValRemplisRouge = 0;
		int valRemplisVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisvert");
		if (valRemplisVert == -1)
			valRemplisVert = ArrayFromREEMAKER.REEvert;
		if (valRemplisVert == INT16_MAX)
			valRemplisVert = 0;
		int valRemplisBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisbleu");
		if (valRemplisBleu == -1)
			valRemplisBleu = ArrayFromREEMAKER.REEbleu;
		if (valRemplisBleu == INT16_MAX)
			valRemplisBleu = 0;
		int valAlignementI = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "alignement");
		if (valAlignementI == INT16_MAX)
			valAlignementI = 0;
		int valAlignementLargeurI = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "alignlargeur");
		if (valAlignementLargeurI == INT16_MAX)
			valAlignementLargeurI = 0;//Defaut gauche
		int valAlignementHauteurI = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "alignhauteur");
		if (valAlignementHauteurI == INT16_MAX)
			valAlignementHauteurI = 1;//Defaut milieu
		int ValMax = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "max");
		if (ValMax == INT16_MAX)
			ValMax = 4096;
		int ValSplit = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "split");
		if (ValSplit == INT16_MAX)
			ValSplit = 0;
		int ValGColonne = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "nombrecolonne");
		if (ValGColonne == INT16_MAX)
			ValGColonne = 0;
		int ValGLigne = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "nombreligne");
		if (ValGLigne == INT16_MAX)
			ValGLigne = 0;
#pragma endregion
		/*
		* (っ◔◡◔)っ 𝙋𝙍𝙀𝙇𝙊𝘼𝘿 𝘿𝙀𝙎 𝘿𝙊𝙐𝘽𝙇𝙀𝙎
		*/
#pragma region Region_Double
		double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
		if (valEpaisseur == (double)INT16_MAX)
			valEpaisseur = 1.0;
		double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
		if (valDebutX == (double)INT16_MAX)
			valDebutX = 0;
		double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
		if (valDebutY == (double)INT16_MAX)
			valDebutY = 0;
		double valFinX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "finx");
		if (valFinX == (double)INT16_MAX)
			valFinX = 0;
		double valFinY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "finy");
		if (valFinY == (double)INT16_MAX)
			valFinY = 0;

		double valDebutX1 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx1");
		if (valDebutX1 == (double)INT16_MAX)
			valDebutX1 = 0.0;
		double valDebutY1 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty1");
		if (valDebutY1 == (double)INT16_MAX)
			valDebutY1 = 0.0;
		double valDebutX2 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx2");
		if (valDebutX2 == (double)INT16_MAX)
			valDebutX2 = 0.0;
		double valDebutY2 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty2");
		if (valDebutY2 == (double)INT16_MAX)
			valDebutY2 = 0.0;

		double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
		if (valLargeur == (double)INT16_MAX)
			valLargeur = 100.0;
		double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
		if (valHauteur == (double)INT16_MAX)
			valHauteur = 20.0;
		double valTaillePolice = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taillepolice");
		if (valTaillePolice == (double)INT16_MAX)
			valTaillePolice = 10.0;
#pragma endregion
		/*
		* (っ◔◡◔)っ 𝙋𝙍𝙀𝙇𝙊𝘼𝘿 𝘿𝙀𝙎 𝘾𝙃𝘼𝙄𝙉𝙀𝙎
		*/
#pragma region region_String
		std::string valTexte = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "texte"));
		if (valTexte == "<!--CleNonTrouve-->")
			valTexte = " ";
		if (vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTE || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION)
			replaceAll(valTexte, "\n", " ");//On supprime les miltilignes si présentes

		std::string valQuestion = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question"));
		std::string valQuestionAide = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion"));
		if (valQuestionAide == "<!--CleNonTrouve-->")
			valQuestionAide = " ";
		std::string valDefautquestion = "";
		if (valDefautquestion == "<!--CleNonTrouve-->")
			valDefautquestion = " ";

		for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
			if (ListeQuestion[iReponse].IndexQuestion == lArg)
			{
				if (ListeQuestion[iReponse].EstCheckbox)
				{
					valDefautquestion = ListeQuestion[iReponse].CheckboxValue ? "oui" : "non";
				}
				else
					valDefautquestion = from_utf8(StringFromVec(ListeQuestion[iReponse].vDefautQuestion));
				break;
			}
#pragma endregion
		/*
		* (っ◔◡◔)っ 𝙋𝙍𝙀𝙇𝙊𝘼𝘿 𝘿𝙀𝙎 𝙁𝙇𝘼𝙂𝙎
		*/
#pragma region Region_Flag
		bool bGras = RetourneCleBool(vecCommandeList[lArg].mVecCommande, "gras");
		bool bItalic = RetourneCleBool(vecCommandeList[lArg].mVecCommande, "italic");
		bool bGrasItalic = RetourneCleBool(vecCommandeList[lArg].mVecCommande, "grasitalic");
		bool bMono = RetourneCleBool(vecCommandeList[lArg].mVecCommande, "monospace");
		if (vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTE ||
			vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION ||
			vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE ||
			vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION)
		{
			if (bMono)
			{//Monospace
				if (bGras)
				{
					pFontMonoBOLD->SetFontSize((float)GetMaxFontSize(pFontMonoBOLD, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontMonoBOLD);
				}
				else if (bItalic)
				{
					pFontMonoITALIC->SetFontSize((float)GetMaxFontSize(pFontMonoITALIC, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontMonoITALIC);
				}
				else if (bGrasItalic)
				{
					pFontMonoBOLDITALIC->SetFontSize((float)GetMaxFontSize(pFontMonoBOLDITALIC, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontMonoBOLDITALIC);
				}
				else
				{
					pFontMono->SetFontSize((float)GetMaxFontSize(pFontMono, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontMono);
				}
			}
			else
			{//Regular
				if (bGras)
				{
					pFontRegBOLD->SetFontSize((float)GetMaxFontSize(pFontRegBOLD, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontRegBOLD);
				}
				else if (bItalic)
				{
					pFontRegITALIC->SetFontSize((float)GetMaxFontSize(pFontRegITALIC, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontRegITALIC);
				}
				else if (bGrasItalic)
				{
					pFontRegBOLDITALIC->SetFontSize((float)GetMaxFontSize(pFontRegBOLDITALIC, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontRegBOLDITALIC);
				}
				else
				{
					pFontReg->SetFontSize((float)GetMaxFontSize(pFontReg, valLargeur, valHauteur, valTaillePolice,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEQUESTION) ? valDefautquestion : valTexte,
						(vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNEQUESTION || vecCommandeList[lArg].mTypeCommande == TypeCommande::DESSINETEXTEMULTILIGNE) ? 1.1 : 1.0));
					Painter.SetFont(pFontReg);
				}
			}
		}
#pragma endregion
		/*
		* (っ◔◡◔)っ 𝙋𝙍𝙀𝙇𝙊𝘼𝘿 𝘿𝙀𝙎 𝘼𝙇𝙄𝙂𝙉𝙀𝙈𝙀𝙉𝙏𝙎
		*/
#pragma region Region_Alignement
		PoDoFo::EPdfAlignment mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
		PoDoFo::EPdfVerticalAlignment mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Top;
		if (valAlignementLargeurI == 0)
			mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
		if (valAlignementLargeurI == 1)
			mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Center;
		if (valAlignementLargeurI == 2)
			mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Right;
		if (valAlignementHauteurI == 0)
			mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Top;
		if (valAlignementHauteurI == 1)
			mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center;
		if (valAlignementHauteurI == 2)
			mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Bottom;
		if (ValSplit > 1)
		{
			mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Center;
			mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center;
		}
#pragma endregion
		/*
		* (っ◔◡◔)っ 𝙋𝙍𝙀𝙇𝙊𝘼𝘿 𝘿𝙐 𝙋𝘼𝙄𝙉𝙏𝙀𝙍
		*/
#pragma region Region_Painter
		Painter.SetStrokeWidth(valEpaisseur);
#pragma endregion
		/*
		* (っ◔◡◔)っ 𝙎𝙬𝙞𝙩𝙘𝙝 𝙖𝙫𝙚𝙘 𝙡𝙚𝙨 𝙙𝙞𝙛𝙛é𝙧𝙚𝙣𝙩𝙚𝙨 𝙖𝙘𝙩𝙞𝙤𝙣𝙨
		*/
#pragma region Region_Switch_Action_Painter
		switch (vecCommandeList[lArg].mTypeCommande)
		{
		case TypeCommande::DESSINELIGNE:
		{
			{
				TRACE_PDG(L"INFO: DESSINELIGNE ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.DrawLine(valDebutX, PageHeight - valDebutY, valFinX, PageHeight - valFinY);
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEVIDE:
		{
			{
				TRACE_PDG(L"INFO: DESSINERECTANGLEVIDE ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEGRILLE:
		{
			{
				TRACE_PDG(L"INFO: DESSINERECTANGLEGRILLE ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				double GapColonne = valLargeur / ValGColonne;
				double GapLigne = valHauteur / ValGLigne;
				for (size_t i = 1; i < ValGColonne; i++)
					Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, i * GapColonne, valHauteur);
				for (size_t i = 0; i < ValGLigne; i++)
				{
					Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, i * GapLigne);
				}
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEREMPLIS:
		{
			{
				TRACE_PDG(L"INFO: DESSINERECTANGLEREMPLIS ...");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetColor((double)(ValRemplisRouge / 255.0), (double)(valRemplisVert / 255.0), (double)(valRemplisBleu / 255.0));//Couleur ligne
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.FillAndStroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTE:
		{
			int valStyle = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "style");
			{
				TRACE_PDG(L"INFO: DESSINETEXTE ... ");
				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				if (ValSplit < 2)
					Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valTexte), mAlignLargeur, mAlignHauteur, false);
				else
				{
					std::vector<char> _vChar; _vChar.resize(ValSplit);
					memcpy_s(&_vChar[0], ValSplit, &CapTo(valTexte, ValSplit)[0], ValSplit);
					double _gap = valLargeur / ValSplit;
					for (size_t i = 0; i < ValSplit; i++)
						Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX + (i * _gap), PageHeight - valDebutY - valHauteur, _gap, valHauteur), PoDoFo::PdfString(std::string(_vChar.begin() + i, _vChar.begin() + i + 1)), mAlignLargeur, mAlignHauteur, false);
				}
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTEMULTILIGNE:
		{
			{
				TRACE_PDG(L"INFO: DESSINETEXTEMULTILIGNE ... ");
				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valTexte), mAlignLargeur, mAlignHauteur);
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTEQUESTION:
		{
			{
				TRACE_PDG(L"INFO: DESSINETEXTEQUESTION ... ");

				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				if (ValSplit < 2)
					Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valDefautquestion), mAlignLargeur, mAlignHauteur, false);
				else
				{
					std::vector<char> _vChar; _vChar.resize(ValSplit);
					memcpy_s(&_vChar[0], ValSplit, &CapTo(valDefautquestion, ValSplit)[0], ValSplit);
					double _gap = valLargeur / ValSplit;
					for (size_t i = 0; i < ValSplit; i++)
						Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX + (i * _gap), PageHeight - valDebutY - valHauteur, _gap, valHauteur), PoDoFo::PdfString(std::string(_vChar.begin() + i, _vChar.begin() + i + 1)), mAlignLargeur, mAlignHauteur, false);
				}
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTEMULTILIGNEQUESTION:
		{
			{
				TRACE_PDG(L"INFO: DESSINETEXTEMULTILIGNEQUESTION ...");
				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valDefautquestion), mAlignLargeur, mAlignHauteur);
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::INSEREIMAGE:
		{
			std::string valChemin = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "chemin"));
			{
				TRACE_PDG(L"INFO: INSEREIMAGE ... ");
				try
				{
					std::string CheminImage = ConvertWideToANSI(BaseModelePath) + valChemin;
					if (std::filesystem::exists(CheminImage))
					{
						PoDoFo::PdfImage image(&Document);
						image.LoadFromFile(CheminImage.c_str());
						double mScaleW = (valLargeur / image.GetWidth());
						double mScaleH = (valHauteur / image.GetHeight());
						Painter.DrawImage(valDebutX, PageHeight - valDebutY - valHauteur, &image, mScaleW, mScaleH);
					}
					TRACE_PDG(L" effectué\n");
				}
				catch (const std::exception&)
				{
					TRACE_PDG(L" non effectué, erreur de chargement de l'image %s\n", valChemin.c_str());
				}
				catch (const PoDoFo::PdfError&)
				{
					TRACE_PDG(L" non effectué, erreur de chargement de l'image %s\n", valChemin.c_str());
				}
			}
		}
		break;
		case TypeCommande::DESSINECHECKBOX:
		{
			{
				TRACE_PDG(L"INFO: DESSINECHECKBOX ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.DrawLine(valDebutX, PageHeight - valDebutY, valDebutX + valLargeur, PageHeight - valDebutY - valHauteur);
				Painter.Stroke();
				Painter.DrawLine(valDebutX, PageHeight - valDebutY - valHauteur, valDebutX + valLargeur, PageHeight - valDebutY);
				Painter.Stroke();
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINECHECKBOXQUESTION:
		{
			{
				TRACE_PDG(L"INFO: DESSINECHECKBOXQUESTION ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(valEpaisseur);
				if (valDefautquestion == "oui")
				{
					Painter.DrawLine(valDebutX, PageHeight - valDebutY, valDebutX + valLargeur, PageHeight - valDebutY - valHauteur);
					Painter.Stroke();
					Painter.DrawLine(valDebutX, PageHeight - valDebutY - valHauteur, valDebutX + valLargeur, PageHeight - valDebutY);
					Painter.Stroke();
				}
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINEMULTICHECKBOXQUESTION:
		{
			{
				TRACE_PDG(L"INFO: DESSINECHECKBOXQUESTION ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.Rectangle(valDebutX1, PageHeight - valDebutY1 - valHauteur, valLargeur, valHauteur);
				Painter.Rectangle(valDebutX2, PageHeight - valDebutY2 - valHauteur, valLargeur, valHauteur);
				if (valDefautquestion == "oui")
				{
					Painter.DrawLine(valDebutX1, PageHeight - valDebutY1, valDebutX1 + valLargeur, PageHeight - valDebutY1 - valHauteur);
					Painter.Stroke();
					Painter.DrawLine(valDebutX1, PageHeight - valDebutY1 - valHauteur, valDebutX1 + valLargeur, PageHeight - valDebutY1);
					Painter.Stroke();
				}
				else
				{
					Painter.DrawLine(valDebutX2, PageHeight - valDebutY2, valDebutX2 + valLargeur, PageHeight - valDebutY2 - valHauteur);
					Painter.Stroke();
					Painter.DrawLine(valDebutX2, PageHeight - valDebutY2 - valHauteur, valDebutX2 + valLargeur, PageHeight - valDebutY2);
					Painter.Stroke();
				}
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::PAGESUIVANTE:
		{
			Painter.FinishPage();
			PoDoFo::PdfPage* pPage = Document.InsertPage(PoDoFo::PdfRect(0.0, 0.0, 595.0, 842.0), NombrePageCree);
			Painter.SetPage(pPage);
			NombrePageCree++;
		}
		break;
		default:
			break;
		}
#pragma endregion
	}
	return NombrePageCree;
}

inline int PDGHelper::ItemCount()
{
	return vecCommandeList.size();
}
inline int PDGHelper::ItemQuestionCount()
{
	return ListeQuestion.size();
}

inline void PDGHelper::ClearList()
{
	vecCommandeList.clear();
	ListeQuestion.clear();
}

inline std::string PDGHelper::StringFromVec(std::vector<char> vecChar)
{
	int EndPos = -1;
	for (int i = 0; i < vecChar.size(); i++)
	{
		if (vecChar[i] == '\0')
		{
			EndPos = i;
			break;
		}
	}
	if (EndPos == -1)
		EndPos = vecChar.size();
	return std::string(vecChar.begin(), vecChar.begin() + EndPos);
}
//Cap string to limit int
inline std::string PDGHelper::CapTo(std::string _str, int _iMax)
{
	if (_str.length() > _iMax)
		return _str.substr(0, _iMax);
	int MissingSpace = _iMax - _str.length();
	for (size_t i = 0; i < MissingSpace; i++)
	{
		_str += " ";
	}
	return _str;
}
// trim from start(in place)
inline void PDGHelper::ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
inline void PDGHelper::rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
inline void PDGHelper::trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
inline std::string PDGHelper::ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
inline std::string PDGHelper::rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
inline std::string PDGHelper::trim_copy(std::string s) {
	trim(s);
	return s;
}

void PDGHelper::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
std::string PDGHelper::to_utf8(std::string str) {
	return ConvertWideToUtf8(ConvertAnsiToWide(str));
}

std::string PDGHelper::from_utf8(std::string str) {
	return ConvertWideToANSI(ConvertUtf8ToWide(str));
}

inline double PDGHelper::GetMaxFontSize(PoDoFo::PdfFont*& Police, double rectWidth, double rectHeight, double mTaillePolice, std::string Texte, double CorrectionEspace)
{
	double lTaillePolice = mTaillePolice;
	while (true)
	{
		Police->SetFontSize((float)lTaillePolice);
		double h = Police->GetFontMetrics()->GetLineSpacing() /*+ std::abs(Police->GetFontMetrics()->GetDescent())*/;
		double w = Police->GetFontMetrics()->StringWidth(Texte);
		double SurfaceTexte = h * w * CorrectionEspace;
		double SurfaceRectangle = rectWidth * rectHeight;
		if (SurfaceTexte < SurfaceRectangle)
			break;
		lTaillePolice -= 0.1;
	}
	return lTaillePolice;
}

// Retourne la valeur STRING de la clé, si non trouvé, retourne "<!--CleNonTrouve-->"
std::string PDGHelper::RetourneCleStr(std::vector<CmdKeys>& lVecKey, std::string Cle)
{
	for (size_t i = 0; i < lVecKey.size(); i++)
	{
		if (lVecKey[i].Keys == Cle)
		{
			std::string mVal = lVecKey[i].Valeur;
			if (mVal.substr(0, 1) == "\"")
				mVal = mVal.substr(1);
			if (mVal.substr(mVal.length() - 1, 1) == "\"")
				mVal = mVal.substr(0, mVal.length() - 1);
			replaceAll(mVal, "{RetourLigne}", "\n");
			replaceAll(mVal, "{Site}", ArrayFromREEMAKER.ReferenceSite.c_str());
			replaceAll(mVal, "{NumeroTranche}", ArrayFromREEMAKER.NumeroTranche.c_str());
			replaceAll(mVal, "{ReferenceREE}", ArrayFromREEMAKER.ReferenceREE.c_str());
			replaceAll(mVal, "{IndiceREE}", ArrayFromREEMAKER.IndiceREE.c_str());
			return mVal;
			break;
		}
	}
	return "<!--CleNonTrouve-->";
}

// Retourne la valeur DOUBLE de la clé, si non trouvé, retourne (double)INT16_MAX
double PDGHelper::RetourneCleDouble(std::vector<CmdKeys>& lVecKey, std::string Cle)
{
	for (size_t i = 0; i < lVecKey.size(); i++)
	{
		if (lVecKey[i].Keys == Cle)
		{
			return (double)std::atof(lVecKey[i].Valeur.c_str());
			break;
		}
	}
	return (double)INT16_MAX;
}

// Retourne la valeur INT de la clé, si non trouvé, retourne INT16_MAX
int PDGHelper::RetourneCleInt(std::vector<CmdKeys>& lVecKey, std::string Cle)
{
	for (size_t i = 0; i < lVecKey.size(); i++)
	{
		if (lVecKey[i].Keys == Cle)
		{
			return std::atoi(lVecKey[i].Valeur.c_str());
			break;
		}
	}
	return INT16_MAX;
}

// Retourne la valeur BOOL true si la clé existe, sinon retourne false
bool PDGHelper::RetourneCleBool(std::vector<CmdKeys>& lVecKey, std::string Cle)
{
	for (size_t i = 0; i < lVecKey.size(); i++)
	{
		if (lVecKey[i].Keys == Cle)
		{
			return true;
			break;
		}
	}
	return false;
}

std::string PDGHelper::ConvertWideToANSI(const std::wstring& wstr)
{
	int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
	std::string str(count, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
	return str;
}
std::wstring PDGHelper::ConvertAnsiToWide(const std::string& str)
{
	int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);
	std::wstring wstr(count, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);
	return wstr;
}
std::string PDGHelper::ConvertWideToUtf8(const std::wstring& wstr)
{
	int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
	std::string str(count, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
	return str;
}
std::wstring PDGHelper::ConvertUtf8ToWide(const std::string& str)
{
	int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
	std::wstring wstr(count, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
	return wstr;
}

PDGHelper::~PDGHelper()
{
}

bool _tracePDGHELPER(const char* format, ...)
{
	char buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);

	OutputDebugStringA(buffer);

	return true;
}
bool _tracePDGHELPER(const wchar_t* format, ...)
{
	wchar_t buffer[1000];

	va_list argptr;
	va_start(argptr, format);
	wvsprintf(buffer, format, argptr);
	va_end(argptr);

	OutputDebugStringW(buffer);

	return true;
}
