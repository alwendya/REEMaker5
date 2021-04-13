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
/*
Definition Ascent / Descent : https://stackoverflow.com/questions/27631736/meaning-of-top-ascent-baseline-descent-bottom-and-leading-in-androids-font#:~:text=Ascent%20-%20The%20recommended%20distance%20above%20the%20baseline,additional%20space%20to%20add%20between%20lines%20of%20text.
*/

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
		DESSINELIGNE, DESSINERECTANGLEVIDE, DESSINERECTANGLEREMPLIS, DESSINETEXTE, DESSINETEXTEMULTILIGNE, DESSINETEXTEQUESTION, DESSINETEXTEMULTILIGNEQUESTION, INSEREIMAGE, DESSINECHECKBOX, DESSINECHECKBOXQUESTION, PAGESUIVANTE, DESSINEMULTICHECKBOXQUESTION
	};

	struct Question
	{
		int IndexQuestion;
		std::string LaQuestion;
		std::string AideQuestion;
		char DefautQuestion[4096];
		bool Obligatoire = false;
		bool EstCheckbox = false;
		bool CheckboxValue = false;
		bool EstLigneTexte = false;
		bool EstMultiLigneTexte = false;
	};
	struct structReplaceArray
	{
		std::string NumeroTranche = "";
		std::string ReferenceREE = "";
		std::string IndiceREE = "";
		int REErouge = 0;
		int REEvert = 0;
		int REEbleu = 0;

	};
	PDGHelper();
	bool OpenAndParseConfig(std::wstring CheminConfig);
	int DrawOnPage(PoDoFo::PdfPainter&, PoDoFo::PdfDocument&);
	int ItemCount();
	int ItemQuestionCount();
	std::string DocumentOuvertANSI();
	std::string DocumentOuvertUTF8();
	std::wstring DocumentOuvertWIDE();
	void SetBaseModelePath(std::wstring);
	bool SauveDisque(std::wstring);
	void ClearList();
	~PDGHelper();
	std::vector<Question> ListeQuestion;
	structReplaceArray ArrayFromREEMAKER;
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
bool PDGHelper::OpenAndParseConfig(std::wstring CheminConfig)
{
	DocumentOuvertWstring = L"";
	std::ifstream FichierConfig(BaseModelePath + CheminConfig, std::wofstream::in);
	if (FichierConfig.fail())
		return false;
	DocumentOuvertWstring = CheminConfig;
	std::vector<std::string> vecFichierPDG;
	vecFichierPDG.clear();
	for (std::string line; std::getline(FichierConfig, line); )
	{
		if (line.length() > 2)
		{
			if (line.substr(0, 2) == "::")//C'est un commentaire on ignore
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
				//vecCommandeList.push_back(mStructCommand);//Donc on le sauve
			}
		}
		else if (trimmedLine.substr(0, 2) == "--")
		{
			CmdKeys mCmdKeys;
			mCmdKeys.Keys = trimmedLine.substr(2, trimmedLine.find_first_of('=') - 2);
			mCmdKeys.Valeur = trimmedLine.substr(mCmdKeys.Keys.length() + 2 + 1);
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
			mQuestion.Obligatoire = (RetourneCleStr(vecCommandeList[i].mVecCommande, "obligatoire") == "oui");
			mQuestion.LaQuestion = RetourneCleStr(vecCommandeList[i].mVecCommande, "question");
			mQuestion.AideQuestion = RetourneCleStr(vecCommandeList[i].mVecCommande, "aidequestion");
			mQuestion.IndexQuestion = i;
			for (size_t j = 0; j < 4096; j++) // RESET VARIABLE
				mQuestion.DefautQuestion[j] = '\0';
			memcpy_s(mQuestion.DefautQuestion, RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion").size(), &RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion")[0], RetourneCleStr(vecCommandeList[i].mVecCommande, "defautquestion").size());
			
			if (vecCommandeList[i].mTypeCommande == TypeCommande::DESSINECHECKBOXQUESTION || vecCommandeList[i].mTypeCommande == TypeCommande::DESSINEMULTICHECKBOXQUESTION)
			{
				mQuestion.EstCheckbox = true;
				mQuestion.CheckboxValue = (std::string(mQuestion.DefautQuestion) == "oui");
				OutputDebugString(L"");
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

inline bool PDGHelper::SauveDisque(std::wstring FichierSortie)
{

	if (vecCommandeList.size() == 0)
	{
		TRACE_PDG(L"INFO: Aucune commande à dessiner...\n");
		return false;
	}

	std::ofstream Fichier(FichierSortie, std::wofstream::out);
	if (Fichier.fail())
		return false;

	for (size_t lArg = 0; lArg < vecCommandeList.size(); lArg++)
	{
		switch (vecCommandeList[lArg].mTypeCommande)
		{
		case TypeCommande::DESSINELIGNE:
		{
			/*	--debutx=12	--debuty=24	--finx=200	--finy=24	--rouge=255	--vert=255	--bleu=0	--epaisseur=1.5	*/
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valFinX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "finx");
			double valFinY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "finy");
			{
				Fichier << "DESSINELIGNE\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--finx=" << valFinX<< "\n";
				Fichier << "--finy=" << valFinY << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--epaisseur=" << valEpaisseur << "\n";
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEVIDE:
		{
			/*	--debutx=12	--debuty=24	--finx=200	--finy=24	--rouge=255	--vert=255	--bleu=0	--epaisseur=1.5	*/
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			{
				Fichier << "DESSINERECTANGLEVIDE\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--epaisseur=" << valEpaisseur << "\n";
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEREMPLIS:
		{
			/*	--debutx=12	--debuty=24	--finx=200	--finy=24	--rouge=255	--vert=255	--bleu=0	--remplisrouge	--remplisvert	--remplisbleu	--epaisseur=1.5	*/
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			int ValRemplisRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisrouge");
			int valRemplisVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisvert");
			int valRemplisBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisbleu");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			{
				Fichier << "DESSINERECTANGLEREMPLIS\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--remplisrouge=" << ValRemplisRouge << "\n";
				Fichier << "--remplisvert=" << valRemplisVert << "\n";
				Fichier << "--remplisbleu=" << valRemplisBleu << "\n";
				Fichier << "--epaisseur=" << valEpaisseur << "\n";
			}
		}
		break;
		case TypeCommande::DESSINETEXTE:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignement = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignement");
			std::string valTexte = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "texte"));
			{
				Fichier << "DESSINETEXTE\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--taille=" << valTaille << "\n";
				Fichier << "--gras=" << valGras << "\n";
				Fichier << "--alignement=\"" << valAlignement << "\"\n";
				Fichier << "--texte=\"" << valTexte << "\"\n";
			}
		}
		break;
		case TypeCommande::DESSINETEXTEMULTILIGNE:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignLargeur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignlargeur");
			std::string valAlignHauteur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignhauteur");
			std::string valTexte = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "texte"));
			{
				Fichier << "DESSINETEXTEMULTILIGNE\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--taille=" << valTaille << "\n";
				Fichier << "--gras=" << valGras << "\n";
				Fichier << "--alignlargeur=\"" << valAlignLargeur << "\"\n";
				Fichier << "--alignhauteur=\"" << valAlignHauteur << "\"\n";
				Fichier << "--texte=\"" << valTexte << "\"\n";
			}
		}
		break;
		case TypeCommande::DESSINETEXTEQUESTION:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignement = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignement");
			std::string valQuestion = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question"));
			std::string valQuestionAide = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion"));
			std::string valObligatoire = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "obligatoire"));
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}

			{
				Fichier << "DESSINETEXTEQUESTION\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--taille=" << valTaille << "\n";
				Fichier << "--gras=" << valGras << "\n";
				Fichier << "--alignement=\"" << valAlignement << "\"\n";

				Fichier << "--question=\"" << valQuestion << "\"\n";
				Fichier << "--aidequestion=\"" << valQuestionAide << "\"\n";
				Fichier << "--defautquestion=\"" << valDefautquestion << "\"\n";
				Fichier << "--obligatoire=\"" << valObligatoire << "\"\n";
			}
		}
		break;
		case TypeCommande::DESSINETEXTEMULTILIGNEQUESTION:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignLargeur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignlargeur");
			std::string valAlignHauteur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignhauteur");
			std::string valQuestion = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question"));
			std::string valQuestionAide = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion"));
			std::string valObligatoire = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "obligatoire"));
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}
			{
				Fichier << "DESSINETEXTEMULTILIGNEQUESTION\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--taille=" << valTaille << "\n";
				Fichier << "--gras=" << valGras << "\n";
				Fichier << "--alignlargeur=\"" << valAlignLargeur << "\"\n";
				Fichier << "--alignhauteur=\"" << valAlignHauteur << "\"\n";

				Fichier << "--question=\"" << valQuestion << "\"\n";
				Fichier << "--aidequestion=\"" << valQuestionAide << "\"\n";
				Fichier << "--defautquestion=\"" << valDefautquestion << "\"\n";
				Fichier << "--obligatoire=\"" << valObligatoire << "\"\n";
			}
		}
		break;
		case TypeCommande::INSEREIMAGE:
		{
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			std::string valChemin = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "chemin"));
			{
				Fichier << "INSEREIMAGE\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--chemin=\"" << valChemin << "\"\n";
			}
		}
		break;
		case TypeCommande::DESSINECHECKBOX:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			{
				Fichier << "DESSINECHECKBOX\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--epaisseur=\"" << valEpaisseur << "\"\n";
				int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
				int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
				int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			}
		}
		break;
		case TypeCommande::DESSINECHECKBOXQUESTION:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			std::string valQuestion = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question"));
			std::string valQuestionAide = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion"));
			std::string valObligatoire = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "obligatoire"));
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}
			{
				Fichier << "DESSINECHECKBOXQUESTION\n";
				Fichier << "--debutx=" << valDebutX << "\n";
				Fichier << "--debuty=" << valDebutY << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--epaisseur=" << valEpaisseur << "\n";

				Fichier << "--question=\"" << valQuestion << "\"\n";
				Fichier << "--aidequestion=\"" << valQuestionAide << "\"\n";
				Fichier << "--defautquestion=\"" << valDefautquestion << "\"\n";
				Fichier << "--obligatoire=\"" << valObligatoire << "\"\n";
			}
		}
		break;
		case TypeCommande::DESSINEMULTICHECKBOXQUESTION:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX1 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx1");
			double valDebutY1 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty1");
			double valDebutX2 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx2");
			double valDebutY2 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty2");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			std::string valQuestion = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question"));
			std::string valQuestionAide = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion"));
			std::string valObligatoire = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "obligatoire"));
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}
			{
				Fichier << "DESSINECHECKBOXQUESTION\n";
				Fichier << "--debutx1=" << valDebutX1 << "\n";
				Fichier << "--debuty1=" << valDebutY1 << "\n";
				Fichier << "--debutx2=" << valDebutX2 << "\n";
				Fichier << "--debuty2=" << valDebutY2 << "\n";
				Fichier << "--largeur=" << valLargeur << "\n";
				Fichier << "--hauteur=" << valHauteur << "\n";
				Fichier << "--rouge=" << ValRouge << "\n";
				Fichier << "--vert=" << valVert << "\n";
				Fichier << "--bleu=" << valBleu << "\n";
				Fichier << "--epaisseur=" << valEpaisseur << "\n";


				Fichier << "--question=\"" << valQuestion << "\"\n";
				Fichier << "--aidequestion=\"" << valQuestionAide << "\"\n";
				Fichier << "--defautquestion=\"" << valDefautquestion << "\"\n";
				Fichier << "--obligatoire=\"" << valObligatoire << "\"\n";
			}
		}
		break;
		case TypeCommande::PAGESUIVANTE:
		{
			Fichier << "PAGESUIVANTE\n";
		}
		break;
		default:
			break;
		}
	}


	return true;


}

inline int PDGHelper::DrawOnPage(PoDoFo::PdfPainter& Painter, PoDoFo::PdfDocument& Document)
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
		Chemin de base de l'executable, fini par \
	*/
	std::string CheminBASE(path);
	CheminBASE = CheminBASE.substr(0, CheminBASE.find_last_of("\\") + 1);
	std::string CheminDroidSans = CheminBASE + "DroidSans.ttf";
	std::string CheminDroidSansBOLD = CheminBASE + "DroidSans-Bold.ttf";
	if (!std::filesystem::exists(CheminDroidSans))
		return 0;
	if (!std::filesystem::exists(CheminDroidSansBOLD))
		return 0;

	PoDoFo::PdfFont* pFont = Document.CreateFontSubset("Droid Sans", false, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminDroidSans.c_str());
	PoDoFo::PdfFont* pFontBOLD = Document.CreateFontSubset("Droid Sans Bold", true, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), CheminDroidSansBOLD.c_str());
	if (pFont == NULL || pFontBOLD == NULL)
	{
		TRACE_PDG(L"ERROR: pFont ou pFontBOLD n'a pu être chargé\n");
		return 0;
	}
	double PageWidth = Painter.GetPage()->GetPageSize().GetWidth();
	double PageHeight = Painter.GetPage()->GetPageSize().GetHeight();

	for (size_t lArg = 0; lArg < vecCommandeList.size(); lArg++)
	{
		switch (vecCommandeList[lArg].mTypeCommande)
		{
		case TypeCommande::DESSINELIGNE:
		{
			/*	--debutx=12	--debuty=24	--finx=200	--finy=24	--rouge=255	--vert=255	--bleu=0	--epaisseur=1.5	*/
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valFinX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "finx");
			double valFinY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "finy");
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert= ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINELIGNE ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(valEpaisseur);
				Painter.DrawLine(valDebutX, PageHeight - valDebutY, valFinX, PageHeight - valFinY);
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEVIDE:
		{
			/*	--debutx=12	--debuty=24	--finx=200	--finy=24	--rouge=255	--vert=255	--bleu=0	--epaisseur=1.5	*/
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINERECTANGLEVIDE ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(valEpaisseur);
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINERECTANGLEREMPLIS:
		{
			/*	--debutx=12	--debuty=24	--finx=200	--finy=24	--rouge=255	--vert=255	--bleu=0	--remplisrouge	--remplisvert	--remplisbleu	--epaisseur=1.5	*/
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			int ValRemplisRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisrouge");
			int valRemplisVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisvert");
			int valRemplisBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "remplisbleu");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				if (ValRemplisRouge == -1 && valRemplisVert == -1 && valRemplisBleu == -1)
				{
					ValRemplisRouge = ArrayFromREEMAKER.REErouge;
					valRemplisVert = ArrayFromREEMAKER.REEvert;
					valRemplisBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINERECTANGLEREMPLIS ...");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetColor((double)(ValRemplisRouge / 255.0), (double)(valRemplisVert / 255.0), (double)(valRemplisBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(valEpaisseur);
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.FillAndStroke();
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTE:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignement = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignement");
			std::string valTexte = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "texte"));
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINETEXTE ... ");
				PoDoFo::EPdfAlignment mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				if (valAlignement == "gauche")
					mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				if (valAlignement == "centre")
					mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Center;
				if (valAlignement == "droite")
					mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Right;

				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				std::string lValTexte = valTexte;
				replaceAll(lValTexte, "\n", "");
				if (valGras == 1)
				{
					pFontBOLD->SetFontSize((float)GetMaxFontSize(pFontBOLD, valLargeur, valHauteur, valTaille, lValTexte, 1.0));
					Painter.SetFont(pFontBOLD);
				}
				else
				{
					pFont->SetFontSize((float)GetMaxFontSize(pFont, valLargeur, valHauteur, valTaille, lValTexte, 1.0));
					Painter.SetFont(pFont);
				}
				Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valTexte), mAlign, PoDoFo::ePdfVerticalAlignment_Center, false);
#ifdef TextDebug
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(1.0);
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Dash);
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Solid);
#endif // debugtext
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTEMULTILIGNE:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignLargeur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignlargeur");
			std::string valAlignHauteur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignhauteur");
			std::string valTexte = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "texte"));
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINETEXTEMULTILIGNE ... ");
				PoDoFo::EPdfAlignment mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				PoDoFo::EPdfVerticalAlignment mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Top;
				if (valAlignLargeur == "gauche")
					mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				if (valAlignLargeur == "centre")
					mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Center;
				if (valAlignLargeur == "droite")
					mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Right;
				if (valAlignHauteur == "haut")
					mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Top;
				if (valAlignHauteur == "bas")
					mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Bottom;
				if (valAlignHauteur == "milieu")
					mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center;

				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				std::string lValTexte = valTexte;
				replaceAll(lValTexte, "\n", "");
				if (valGras == 1)
				{
					pFontBOLD->SetFontSize((float)GetMaxFontSize(pFontBOLD, valLargeur, valHauteur, valTaille, lValTexte));
					Painter.SetFont(pFontBOLD);
				}
				else
				{
					pFont->SetFontSize((float)GetMaxFontSize(pFont, valLargeur, valHauteur, valTaille, lValTexte));
					Painter.SetFont(pFont);
				}
				Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valTexte), mAlignLargeur, mAlignHauteur);
#ifdef TextDebug
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(1.0);
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Dash);
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Solid);
#endif // debugtext
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTEQUESTION:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignement = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignement");
			std::string valQuestion = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question"));
			std::string valQuestionAide = from_utf8(RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion"));
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}

			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINETEXTEQUESTION ... ");
				PoDoFo::EPdfAlignment mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				if (valAlignement == "gauche")
					mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				if (valAlignement == "centre")
					mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Center;
				if (valAlignement == "droite")
					mAlign = PoDoFo::EPdfAlignment::ePdfAlignment_Right;

				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				std::string lValTexte = valDefautquestion;
				replaceAll(lValTexte, "\n", "");
				if (valGras == 1)
				{
					pFontBOLD->SetFontSize((float)GetMaxFontSize(pFontBOLD, valLargeur, valHauteur, valTaille, lValTexte, 1.0));
					Painter.SetFont(pFontBOLD);
				}
				else
				{
					pFont->SetFontSize((float)GetMaxFontSize(pFont, valLargeur, valHauteur, valTaille, lValTexte, 1.0));
					Painter.SetFont(pFont);
				}
				Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valDefautquestion), mAlign, PoDoFo::ePdfVerticalAlignment_Center, false);
#ifdef TextDebug
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(1.0);
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Dash);
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Solid);
#endif // debugtext
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::DESSINETEXTEMULTILIGNEQUESTION:
		{
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valTaille = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "taille");
			int valGras = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "gras");
			std::string valAlignLargeur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignlargeur");
			std::string valAlignHauteur = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "alignhauteur");
			std::string from_utf8valQuestion = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question");
			std::string from_utf8valQuestionAide = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion");
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINETEXTEMULTILIGNEQUESTION ...");
				PoDoFo::EPdfAlignment mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				PoDoFo::EPdfVerticalAlignment mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Top;
				if (valAlignLargeur == "gauche")
					mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Left;
				if (valAlignLargeur == "centre")
					mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Center;
				if (valAlignLargeur == "droite")
					mAlignLargeur = PoDoFo::EPdfAlignment::ePdfAlignment_Right;
				if (valAlignHauteur == "haut")
					mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Top;
				if (valAlignHauteur == "bas")
					mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Bottom;
				if (valAlignHauteur == "milieu")
					mAlignHauteur = PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center;

				Painter.SetColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				std::string lValTexte = valDefautquestion;
				replaceAll(lValTexte, "\n", "");
				if (valGras == 1)
				{
					pFontBOLD->SetFontSize((float)GetMaxFontSize(pFontBOLD, valLargeur, valHauteur, valTaille, lValTexte));
					Painter.SetFont(pFontBOLD);
				}
				else
				{
					pFont->SetFontSize((float)GetMaxFontSize(pFont, valLargeur, valHauteur, valTaille, lValTexte));
					Painter.SetFont(pFont);
				}
				Painter.DrawMultiLineText(PoDoFo::PdfRect(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur), PoDoFo::PdfString(valDefautquestion), mAlignLargeur, mAlignHauteur);
#ifdef TextDebug
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(1.0);
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Dash);
				Painter.Rectangle(valDebutX, PageHeight - valDebutY - valHauteur, valLargeur, valHauteur);
				Painter.Stroke();
				Painter.SetStrokeStyle(PoDoFo::ePdfStrokeStyle_Solid);
#endif // debugtext
				TRACE_PDG(L" effectué\n");
			}
		}
		break;
		case TypeCommande::INSEREIMAGE:
		{
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
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
						Painter.DrawImage(valDebutX, PageHeight - valDebutY - valLargeur, &image, mScaleW, mScaleH);
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
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINECHECKBOX ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(valEpaisseur);
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
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx");
			double valDebutY = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			std::string valQuestion = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question");
			std::string valQuestionAide = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion");
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
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
			int ValRouge = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "rouge");
			int valVert = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "vert");
			int valBleu = RetourneCleInt(vecCommandeList[lArg].mVecCommande, "bleu");
			double valDebutX1 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx1");
			double valDebutY1 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty1");
			double valDebutX2 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debutx2");
			double valDebutY2 = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "debuty2");
			double valLargeur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "largeur");
			double valHauteur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "hauteur");
			double valEpaisseur = RetourneCleDouble(vecCommandeList[lArg].mVecCommande, "epaisseur");
			std::string valQuestion = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "question");
			std::string valQuestionAide = RetourneCleStr(vecCommandeList[lArg].mVecCommande, "aidequestion");
			std::string valDefautquestion = "";
			for (size_t iReponse = 0; iReponse < ListeQuestion.size(); iReponse++)
				if (ListeQuestion[iReponse].IndexQuestion == lArg)
				{
					valDefautquestion = from_utf8(std::string(ListeQuestion[iReponse].DefautQuestion));
					break;
				}
			{
				if (ValRouge == -1 && valVert == -1 && valBleu == -1)
				{
					ValRouge = ArrayFromREEMAKER.REErouge;
					valVert = ArrayFromREEMAKER.REEvert;
					valBleu = ArrayFromREEMAKER.REEbleu;
				}
				TRACE_PDG(L"INFO: DESSINECHECKBOXQUESTION ... ");
				Painter.SetStrokingColor((double)(ValRouge / 255.0), (double)(valVert / 255.0), (double)(valBleu / 255.0));//Couleur ligne
				Painter.SetStrokeWidth(valEpaisseur);
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
			PoDoFo::PdfPage* pPage = Document.InsertPage(PoDoFo::PdfRect(0.0, 0.0, 595.0, 842.0),NombrePageCree);
			Painter.SetPage(pPage);
			NombrePageCree++;
		}
		break;
		default:
			break;
		}
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
			replaceAll(mVal, "{NumeroTranche}", ArrayFromREEMAKER.NumeroTranche.c_str());
			replaceAll(mVal, "{ReferenceREE}", ArrayFromREEMAKER.ReferenceREE.c_str());
			replaceAll(mVal, "{IndiceREE}", ArrayFromREEMAKER.IndiceREE.c_str());
			return mVal;
			break;
		}
	}
	return "";
}
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
	return 0.0f;
}
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
	return 0;
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
