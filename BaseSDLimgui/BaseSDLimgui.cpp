﻿#include "Header_Main.h"
#include "Header__Font.h"
#include "Header__Function.h"
#include "Header__ImGUI_addon.h"
#include "Header__CryptoZstd.h"
#include "Header__WinToast.h"
using namespace std;

/*
* Prototype
*/
bool SauveParametres();
bool ChargeParametres();
wstring generate(int max_length);


class PDFError :public exception
{
public:
	const char* what() const throw()
	{
		return "Erreur de PDF";
	}
};

struct BenchStruct
{
	int Level = 0;
	UINT64 TailleOriginal = 0;
	UINT64 TailleCompressé = 0;
	double Ratio = 0.;
	double TempsCompression = 0.;
	double DebitMbs = 0.;
};
std::vector<BenchStruct> vecBench;
constexpr auto OpEnCoursWidth = 200.0f;
constexpr auto OpEnCoursHeight = 200.0f;
constexpr auto mFenWidth = 1280.0f;
constexpr auto mFenHeight = 720.0f;
// Mes vars internes
bool show_demo_window = false;
ImVec4 clear_color = IMVEC4_COL16(115, 135, 150, 255);

static std::string MessageOPENCours("Analyse du document en cours");
static std::string MessagePercentOPENCours("12 %");
static wstring CheminBASE(L"");
static wstring CheminFont(L"");
static wstring CheminPDFTK(L"");
static wstring FichierPDFsortie(L"");
static vector<PoDoFo::PdfRect> vecMediaBox;
static vector<int> vecRotation;
static char  txtSpinner[2] = "\0";
static char NomSite[32];
static char TrancheCode[10][6];
static char strREFERENCEREE[32] = "";
static char strINDICEREE[5] = "";
static bool TrancheSelect[10];
static bool ThemeSombre = false;
static bool tabFolioter = true;
static bool tabPageDeGarde = true;
static bool tabParametre = true;
static int radioTotalPartiel = 0;
/*  0=HautGauche 1=HautDroite(DEFAUT) 2=BasGauche 3=BasDroite*/
static int radioEmplacementTampon = 1;
static ImU16 margeEmplacementTamponX = 20;
static ImU16 margeEmplacementTamponY = 20;
static const ImU16   u16_one = 1, u16_fast = 10;//Réglage du Scalar
static ImU16  u16_PageDebut = 1;
static ImU16  u16_PageFin = 1;
static ImU16  u16_PremierNumero = 3;

struct Couleur
{
	string NomCouleur = "";
	bool Locked = false;
	ImVec4 CodeCouleur = { 0.0f,0.0f,0.0f,0.0f };
};
vector<Couleur> ListeCouleur;
static float sListeCouleurTranche[10][4] = { 0.0f, 0.0f, 0.0f, 1.0f };

// Main code
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/* A 256 bit key :  32char = 256bits*/
	std::string key(AY_OBFUSCATE("QxXtiJw^&gh79ZkBDC$bLMESpYJ8VX&X"));
	/* A 128 bit IV :  16char = 128bits*/
	std::string iv(AY_OBFUSCATE("Vmn8XqE^2jWxDKAv"));

	TCHAR path[4096];
	DWORD length;
	length = GetModuleFileName(NULL, path, 4096);
	/*
		Chemin de base de l'executable, fini par \
	*/
	CheminBASE = path;
	CheminBASE = CheminBASE.substr(0, CheminBASE.find_last_of(L"\\") + 1);

	CheminFont = CheminBASE + L"REEMAKER.ttf";//J'utilise Droid Sans pour des histoires de droits
	CheminPDFTK = CheminBASE + L"pdftk.exe";


	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("REEMaker 5 - A new hope", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	ImGui::/*StyleColorsLightGreen*/StyleColorBlackWhite();
	SDL_SetWindowMinimumSize(window, 1024, 720);
	//FONT AWESOME merged with Droid
	ImFont* MYFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(droidSansFont_compressed_data, droidSansFont_compressed_size, 14.0f);
	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 }; // Will not be copied by AddFont* so keep in scope.
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesome4_compressed_data, FontAwesome4_compressed_size, 14.0f, &config, icons_ranges);// Merge into first font
	io.Fonts->Build();
	ImFont* MYFont20 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(droidSansFont_compressed_data, droidSansFont_compressed_size, 20.0f);
	ImFont* MYFont14bold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(DroidSansBold_compressed_data, DroidSansBold_compressed_size, 14.0f);
	ImFont* MYFont10bold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(DroidSansBold_compressed_data, DroidSansBold_compressed_size, 10.0f);

	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();
	static bool AfficheFenetreSpinner = false;
	static bool AfficheFenetreSpinnerthreadFolioSansPDG = false;

	static bool BoutonContinueCheminProcedure = false;
	static bool BoutonContinueReferenceProcedure = true;
	static bool AfficheReferenceProcedure = false;
	static bool BoutonContinueTrancheProcedure = true;
	static bool AfficheTrancheProcedure = false;
	static bool BoutonContinueFolioProcedure = true;
	static bool AfficheFolioProcedure = false;
	static std::string CheminPDF("");
	static std::string InformationPDF("");
	static std::wstring wCheminPDF(L"");
	ListeCouleur.clear();
	Couleur mCouleur;
	{
		mCouleur.NomCouleur = ICON_FA_LOCK " Rouge";
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 1.0f,0.0f,0.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
		mCouleur.NomCouleur = ICON_FA_LOCK " Vert";
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 0.0f,1.0f,0.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
		mCouleur.NomCouleur = ICON_FA_LOCK " Bleu";
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 0.0f,0.0f,1.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
		mCouleur.NomCouleur = ICON_FA_LOCK " Noir";
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 0.0f,0.0f,0.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
	}
	/*
	* Charge les paramètres
	*/
	ChargeParametres();
	/*
	* Main loop
	*/


	bool done = false;
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
		{
			sprintf_s(txtSpinner, 2, "%c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
			txtSpinner[1] = '\0';
			/*
				╔═╗╔═╗╔╗╔╔═╗╔╦╗╦═╗╔═╗  ╔═╗╦═╗╦╔╗╔╔═╗╦╔═╗╔═╗╦  ╔═╗
				╠╣ ║╣ ║║║║╣  ║ ╠╦╝║╣   ╠═╝╠╦╝║║║║║  ║╠═╝╠═╣║  ║╣
				╚  ╚═╝╝╚╝╚═╝ ╩ ╩╚═╚═╝  ╩  ╩╚═╩╝╚╝╚═╝╩╩  ╩ ╩╩═╝╚═╝
			*/
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			int width;
			int height;
			SDL_GetWindowSize(window, &width, &height);
			ImGui::SetNextWindowSize(ImVec2(width, height));
			ImGui::Begin("Fenètre principale", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);// Create a window called "Hello, world!" and append into it.

			if (ImGui::BeginTabBar("TabBar"), ImGuiTabBarFlags_FittingPolicyScroll)
			{
				if (ImGui::BeginTabItem(ICON_FA_FILE_PDF_O " Folioter un REE"))
				{
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Chemin de la procédure :");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(u8"Chemin de la procédure :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Cliquer sur le bouton Rechercher pour sélectionner la procédure PDF à passer REE."); ImGui::Text("");

					float fenInsideWidth = ImGui::GetWindowContentRegionWidth();

					static char strCHEMINPROCEDURE[4096] = "";
					ImGui::SetNextItemWidth(fenInsideWidth - 280.0f - 8.0f);
					ImGui::InputTextWithHint(u8"##TextCheminProcedure", u8"Cliquer sur rechercher pour sélectionner la procédure à folioter", strCHEMINPROCEDURE, IM_ARRAYSIZE(strCHEMINPROCEDURE), ImGuiInputTextFlags_ReadOnly);

					ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					if (ImGui::Button(ICON_FA_FOLDER_OPEN "  Rechercher##OuvreProcedure", ImVec2(280.0f, 22.0f)))
					{
						AfficheFenetreSpinner = true;
						MessageOPENCours = fmt::format("Analyse du document PDF.");
						MessagePercentOPENCours = "";
						ImGui::OpenPopup("Avancement##TABfolioteruneprocedure");


						HRESULT hr;
						CComPtr<IFileOpenDialog> pDlg;
						COMDLG_FILTERSPEC aFileTypes[] = {
							{ L"Fichier PDF Acrobat", L"*.pdf" },
							{ L"Tout les fichiers", L"*.*" }
						};
						hr = pDlg.CoCreateInstance(__uuidof(FileOpenDialog));
						if (FAILED(hr))
							return -1;
						pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
						pDlg->SetTitle(L"Rechercher la procédure PDF à passer REE");
						hr = pDlg->Show(/*m_hWnd*/NULL);
						if (SUCCEEDED(hr))
						{
							CComPtr<IShellItem> pItem;
							hr = pDlg->GetResult(&pItem);
							if (SUCCEEDED(hr))
							{
								LPOLESTR pwsz = NULL;
								hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
								if (SUCCEEDED(hr))
								{
									CheminPDF = ConvertWideToUtf8(wstring(pwsz));
									wCheminPDF = wstring(pwsz);
									CoTaskMemFree(pwsz);
									std::thread t([]()
										{
											MessageOPENCours = fmt::format("Analyse du document PDF..");
											MessagePercentOPENCours = "";
											try
											{
												vecMediaBox.clear();
												vecRotation.clear();
												PoDoFo::PdfMemDocument documentSource(wCheminPDF.c_str());
												int nbPages = documentSource.GetPageCount();
												for (size_t i = 0; i < nbPages; i++)
												{
													PoDoFo::PdfPage* pPage = documentSource.GetPage(i);
													vecMediaBox.push_back(pPage->GetMediaBox());
													vecRotation.push_back(pPage->GetRotation());
													MessageOPENCours = fmt::format(string("Analyse du document PDF " + string(txtSpinner)));
													MessagePercentOPENCours = fmt::format("{}%", (int)(((float)i / nbPages) * 100.0f) + 1);
												}
												InformationPDF = fmt::format(u8" {} page(s) pour {} Mo", nbPages, filesystem::file_size(wCheminPDF) / 1024 / 1024);
												memcpy_s(strCHEMINPROCEDURE, CheminPDF.size(), &CheminPDF[0], CheminPDF.size());
												strCHEMINPROCEDURE[CheminPDF.size()] = '\0';
												this_thread::sleep_for(500ms);
												BoutonContinueCheminProcedure = true;
											}
											catch (const PoDoFo::PdfError& e)
											{
												if (e.GetError() == PoDoFo::ePdfError_FileNotFound)
													InformationPDF = fmt::format(u8"Erreur : Le fichier est introuvable");
												else if (e.GetError() == PoDoFo::ePdfError_BrokenFile)
													InformationPDF = fmt::format(u8"Erreur : Le fichier est endommagé");
												else if (e.GetError() == PoDoFo::ePdfError_NoPdfFile)
													InformationPDF = fmt::format(u8"Erreur : Le fichier n'est pas un fichier PDF");
												else
													InformationPDF = fmt::format(u8"Erreur à l'ouverture : {}", e.what());
											}
											//On reset le parcours
											u16_PageDebut = 1;
											u16_PageFin = vecRotation.size();
											BoutonContinueReferenceProcedure = true;
											AfficheReferenceProcedure = false;
											BoutonContinueTrancheProcedure = true;
											AfficheTrancheProcedure = false;
											BoutonContinueFolioProcedure = true;
											AfficheFolioProcedure = false;
											AfficheFenetreSpinner = false;
										}
									);
									t.detach();
								}
							}
						}
						else
						{//Pressez sur annuler
							AfficheFenetreSpinner = false;
						}
					}
					ImGui::PopStyleVar();


					ImGui::Text(u8"Information sur la procédure sélectionné : ");
					ImGui::SameLine();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(InformationPDF.c_str());
					ImGui::PopFont();

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					if (ImGuiAl::Button(ICON_FA_FORWARD " Continuer##BoutonContinueCheminProcedure", BoutonContinueCheminProcedure))
					{
						BoutonContinueCheminProcedure = false;
						AfficheReferenceProcedure = true;
					}
					ImGui::PopStyleVar();

					if (AfficheReferenceProcedure)
					{
						ImGui::Separator();
						ImGui::PushFont(MYFont14bold);
						ImGui::Text(u8"Référence du tampon REE :");
						ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(u8"Référence du tampon REE :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
						ImGui::PopFont();
						ImGui::SameLine(); HelpMarker(u8"Ici, vous allez saisir la référence de la procédure à passer REE.\nIl faut saisir le site, le palier, REE, chantier ou non, système élémentaire, numéro de la procédure"); ImGui::Text("");
						/****************/
						ImGui::Text(u8"Nom du site : ");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(250.0f);
						if (AfficheFolioProcedure)
							ImGui::PushFont(MYFont14bold);
						ImGui::InputTextWithHint("##NomDuSite", "ex. Flamanville", NomSite, IM_ARRAYSIZE(NomSite), AfficheFolioProcedure ? ImGuiInputTextFlags_ReadOnly : 0);
						if (AfficheFolioProcedure)
							ImGui::PopFont();
						ImGui::SameLine();
						/****************/
						ImGui::Text(u8"Référence de la procédure : ");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(250.0f);
						if (AfficheFolioProcedure)
							ImGui::PushFont(MYFont14bold);
						ImGui::InputTextWithHint("##ReferenceProcedure", "ex. YR REE LHR 104", strREFERENCEREE, IM_ARRAYSIZE(strREFERENCEREE), AfficheFolioProcedure ? ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsUppercase : ImGuiInputTextFlags_CharsUppercase);
						if (AfficheFolioProcedure)
							ImGui::PopFont();
						ImGui::SameLine();
						/****************/
						ImGui::Text(u8"  Indice de la procédure : ");
						ImGui::SameLine();
						if (AfficheFolioProcedure)
							ImGui::PushFont(MYFont14bold);
						ImGui::InputTextWithHint("##IndiceProcedure", "ex. A ou PREL", strINDICEREE, IM_ARRAYSIZE(strINDICEREE), AfficheFolioProcedure ? ImGuiInputTextFlags_ReadOnly : 0);
						if (AfficheFolioProcedure)
							ImGui::PopFont();
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						if (!AfficheFolioProcedure)
							if (string(strREFERENCEREE).size() > 0 && string(strINDICEREE).size() > 0 && string(NomSite).size() > 0)
								BoutonContinueReferenceProcedure = true;
							else
								BoutonContinueReferenceProcedure = false;
						if (ImGuiAl::Button(ICON_FA_FORWARD " Continuer##BoutonContinueReferenceProcedure", BoutonContinueReferenceProcedure))
						{
							BoutonContinueReferenceProcedure = false;
							AfficheFolioProcedure = true;
						}
						ImGui::PopStyleVar();
					}
					if (AfficheFolioProcedure)
					{
						ImGui::Separator();
						ImGui::PushFont(MYFont14bold);
						ImGui::Text(u8"Etendue du foliotage :");
						ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(u8"Etendue du foliotage :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
						ImGui::PopFont();
						ImGui::SameLine(); HelpMarker(u8"Ici vous devez choisir si la procédure est folioté en totalitée ou partiellement.\nVous allez aussi définir le premier numéro apparaissant sur le premier folio tamponné."); ImGui::Text("");

						if (ImGui::BeginTable("##TableTotalPartiel", 2, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, 55.0f)))
						{

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::RadioButton("Foliotage total", &radioTotalPartiel, 0); ImGui::SameLine();

							ImGui::TableSetColumnIndex(1);
							ImGui::RadioButton("Foliotage partiel", &radioTotalPartiel, 1);
							if (radioTotalPartiel == 1)
							{
								ImGui::Text(u8"De la page "); ImGui::SameLine();
								ImGui::SetNextItemWidth(100.0f);
								if (ImGui::InputScalar("##ScalarDebut", ImGuiDataType_U16, &u16_PageDebut, &u16_one, &u16_fast, "%u", BoutonContinueFolioProcedure ? 0 : ImGuiInputTextFlags_ReadOnly))
								{
									if (u16_PageDebut <= 0)
										u16_PageDebut = 1;
								}
								ImGui::SameLine();
								ImGui::Text(u8"à la page"); ImGui::SameLine();
								ImGui::SetNextItemWidth(100.0f);
								if (ImGui::InputScalar("##ScalarFin", ImGuiDataType_U16, &u16_PageFin, &u16_one, &u16_fast, "%u", BoutonContinueFolioProcedure ? 0 : ImGuiInputTextFlags_ReadOnly))
								{
									if (u16_PageFin > vecRotation.size())
										u16_PageFin = vecRotation.size();
								}

							}
							ImGui::EndTable();
						}

						ImGui::Text(u8"Le folio du premier tampon sera : "); ImGui::SameLine();
						if (ImGui::InputScalar("##ScalarFin", ImGuiDataType_U16, &u16_PremierNumero, &u16_one, &u16_fast, "%u", BoutonContinueFolioProcedure ? 0 : ImGuiInputTextFlags_ReadOnly))
						{

						}

						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						if (ImGuiAl::Button(ICON_FA_FORWARD " Continuer##BoutonContinueFolioProcedure", BoutonContinueFolioProcedure))
						{
							BoutonContinueFolioProcedure = false;
							AfficheTrancheProcedure = true;
						}
						ImGui::PopStyleVar();

					}

					if (AfficheTrancheProcedure)
					{
						ImGui::Separator();
						ImGui::PushFont(MYFont14bold);
						ImGui::Text(u8"Choix des tranches et réglages des cycles :");
						ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(
							u8"Choix des tranches et réglages des cycles :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
						ImGui::PopFont();
						ImGui::SameLine(); HelpMarker(u8"Ici, vous allez cocher les tranches applicables au REE et saisir le code projet.\nLe code projet est sous la forme XYYZZ avec :\nX = Le type de Cycle\nYY = L'année du cycle\nZZ = L'index du cycle"); ImGui::Text("");
						if (ImGui::BeginTable("##tableTranche", 3, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, 250.0f)))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("Tranche 1");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche1", &TrancheSelect[1]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[1])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche1", "ex. C2015", TrancheCode[1], IM_ARRAYSIZE(TrancheCode[1]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche1", "ex. C2015", TrancheCode[1], IM_ARRAYSIZE(TrancheCode[1]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("Tranche 2");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche2", &TrancheSelect[2]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[2])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche2", "ex. C2015", TrancheCode[2], IM_ARRAYSIZE(TrancheCode[2]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche2", "ex. C2015", TrancheCode[2], IM_ARRAYSIZE(TrancheCode[2]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableSetColumnIndex(2);
							ImGui::Text("Tranche 9");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche9", &TrancheSelect[9]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[9])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche9", "ex. C2015", TrancheCode[9], IM_ARRAYSIZE(TrancheCode[9]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche9", "ex. C2015", TrancheCode[9], IM_ARRAYSIZE(TrancheCode[9]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("Tranche 3");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche3", &TrancheSelect[3]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[3])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche3", "ex. C2015", TrancheCode[3], IM_ARRAYSIZE(TrancheCode[3]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche3", "ex. C2015", TrancheCode[3], IM_ARRAYSIZE(TrancheCode[3]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("Tranche 4");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche4", &TrancheSelect[4]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[4])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche4", "ex. C2015", TrancheCode[4], IM_ARRAYSIZE(TrancheCode[4]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche4", "ex. C2015", TrancheCode[4], IM_ARRAYSIZE(TrancheCode[4]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableSetColumnIndex(2);
							ImGui::Text("Tranche 8");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche8", &TrancheSelect[8]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[8])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche8", "ex. C2015", TrancheCode[8], IM_ARRAYSIZE(TrancheCode[8]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche8", "ex. C2015", TrancheCode[8], IM_ARRAYSIZE(TrancheCode[8]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("Tranche 5");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche5", &TrancheSelect[5]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[5])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche5", "ex. C2015", TrancheCode[5], IM_ARRAYSIZE(TrancheCode[5]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche5", "ex. C2015", TrancheCode[5], IM_ARRAYSIZE(TrancheCode[5]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("Tranche 6");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche6", &TrancheSelect[6]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[6])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche6", "ex. C2015", TrancheCode[6], IM_ARRAYSIZE(TrancheCode[6]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche6", "ex. C2015", TrancheCode[6], IM_ARRAYSIZE(TrancheCode[6]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableSetColumnIndex(2);
							ImGui::Text("Tranche 7");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche7", &TrancheSelect[7]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[7])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche7", "ex. C2015", TrancheCode[7], IM_ARRAYSIZE(TrancheCode[7]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche7", "ex. C2015", TrancheCode[7], IM_ARRAYSIZE(TrancheCode[7]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(1);
							ImGui::Text("Tranche 0");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox("##Tranche0", &TrancheSelect[0]);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							if (TrancheSelect[0])
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::InputTextWithHint("##CycleTranche0", "ex. C2015", TrancheCode[0], IM_ARRAYSIZE(TrancheCode[0]), ImGuiInputTextFlags_CharsUppercase);
								ImGui::PopFont();
							}
							else
								ImGui::InputTextWithHint("##CycleTranche0", "ex. C2015", TrancheCode[0], IM_ARRAYSIZE(TrancheCode[0]), ImGuiInputTextFlags_ReadOnly);
							ImGui::Text("");


							ImGui::EndTable();
						}
						int nbTrue = 0;
						for (size_t cTrue = 0; cTrue < 10; cTrue++)
							if (TrancheSelect[cTrue]) nbTrue++;

						if (nbTrue > 0)
							if (ImGui::BeginTable("##tableBoutonFolio", 2, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, 30.0f)))
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
								if (ImGui::Button(u8"Folioter avec une page de garde", ImVec2(-1.0f, 30.0f)))
								{

								}
								ImGui::PopStyleVar();

								ImGui::TableSetColumnIndex(1);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
								if (ImGui::Button(u8"Folioter sans page de garde", ImVec2(-1.0f, 30.0f)))
								{
									bool FoundAfile = false;
									//Sauvegarder sous
									while (true) {

										HRESULT hr;
										CComPtr<IFileSaveDialog> pDlg;
										COMDLG_FILTERSPEC aFileTypes[] = { { L"Fichier PDF", L"*.pdf" } };
										hr = pDlg.CoCreateInstance(__uuidof(FileSaveDialog));
										if (FAILED(hr))
											break;
										pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
										pDlg->SetTitle(L"Enregistrer la procédure foliotée");
										pDlg->SetOkButtonLabel(L"&Sauvegarder");
										wstring CustomCheminPDF = wCheminPDF;
										CustomCheminPDF = CustomCheminPDF.substr(0, CustomCheminPDF.length() - 4);
										CustomCheminPDF += L"_foliotée";
										pDlg->SetFileName(CustomCheminPDF.c_str());
										pDlg->SetDefaultExtension(L"pdf");
										hr = pDlg->Show(NULL);
										if (SUCCEEDED(hr))
										{
											CComPtr<IShellItem> pItem;
											hr = pDlg->GetResult(&pItem);
											if (SUCCEEDED(hr))
											{
												LPOLESTR pwsz = NULL;
												hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
												if (SUCCEEDED(hr))
												{
													FoundAfile = true;
													FichierPDFsortie = wstring(pwsz);
													OutputDebugStringW(FichierPDFsortie.c_str());
													OutputDebugStringW(L"\n");
													CoTaskMemFree(pwsz);
												}
											}
										}
										else
										{
											FoundAfile = false;
											FichierPDFsortie = L"";
										}
										break;
									}

									if (FoundAfile)
									{
										AfficheFenetreSpinnerthreadFolioSansPDG = true;
										MessageOPENCours = fmt::format(u8"Création des tampons {}", string(txtSpinner));
										MessagePercentOPENCours = fmt::format("");
										ImGui::OpenPopup("Avancement##threadFolioSansPDG");

										std::thread t([]()
											{
												///*  CONFIGURATION TAMPON
												//    /---------------------------------------\ 60
												//    |                               |margH  |
												//    | <margL>Site de fla...<margL>  Tr.2    |
												//    |                               |margH  |
												//    |---------------------------------------|h2  40
												//    |                         |     |margH  |
												//    | PNO REE DEL 003 VEC CHA | Indice A0   |
												//    |                         |     |margH  |
												//    |---------------------s1-100------------|h1  20
												//    |               |               |margH  |
												//    | Folio 10000   |      0D2320           |
												//    |               |               |margH  |
												//    \-----------s2-60-----------------------/
												//                                                150
												//*/



												constexpr double TamponLargeur = 150.0;
												constexpr double TamponHauteur = 60.0;
												constexpr double TamponMargH = 7.0;
												constexpr double TamponMargL = 4.0;
												constexpr double TamponH1 = 20.0;
												constexpr double TamponH2 = 40.0;
												constexpr double TamponS1 = 100.0;
												constexpr double TamponS2 = 60.0;
												constexpr double TamponEpaisseur = 1.0;
												constexpr double TamponPolice = 8;

												PDFError exPDFError;




												MessageOPENCours = fmt::format(u8"Création des tampons {}", string(txtSpinner));
												MessagePercentOPENCours = fmt::format("");

												for (size_t t = 0; t < 10; t++)
												{
													if (TrancheSelect[t] == false)
														continue;

													try
													{
														wstring TEMPTampon = filesystem::temp_directory_path().wstring() + generate(16) + L"_tr" + to_wstring(t) + L".pdf";
														{
															PoDoFo::PdfStreamedDocument document(TEMPTampon.c_str());
															PoDoFo::PdfFont* pFont = document.CreateFont("Droid Sans", false, false, true, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), PoDoFo::PdfFontCache::eFontCreationFlags_AutoSelectBase14, true, ConvertWideToANSI(CheminFont).c_str());
															pFont->SetFontSize(TamponPolice);
															size_t mStarting = 0;
															size_t mEnding = vecMediaBox.size();

															if (radioTotalPartiel == 1)//Partiel
															{
																mStarting = u16_PageDebut - 1;
																mEnding = u16_PageFin;
															}

															for (size_t i = mStarting; i < mEnding; i++)
															{
																MessageOPENCours = fmt::format(u8"Création des tampons {}\n\nPage {} / {}", string(txtSpinner), i + 1, vecMediaBox.size());
																MessagePercentOPENCours = fmt::format("Tr.{}", t);
																PoDoFo::PdfPage* pPage = document.CreatePage(vecMediaBox[i]);

																pPage->SetRotation(vecRotation[i]);
																PoDoFo::PdfPainter painter;
																PoDoFo::PdfRect rect(0, 0, TamponLargeur, TamponHauteur);

																if (vecRotation[i] == 90)
																	rect = PoDoFo::PdfRect(0, 0, TamponHauteur, TamponLargeur);
																else if (vecRotation[i] == 270)
																	rect = PoDoFo::PdfRect(0, 0, TamponHauteur, TamponLargeur);

																PoDoFo::PdfXObject xObj(rect, &document);
																painter.SetPage(&xObj);
																/*
																* Matrix work
																* SetTransformationMatrix(a, b, c, d, e, f)
																* double a, b, c, d, e, f;
																* double alpha = AngleRotation;	Pour 90			Pour 180		  Pour 270
																* a = cos (alpha);					  0				  -1				 0
																* b = sin (alpha);					  1				   0			    -1
																* c = -sin(alpha);					 -1				   0			     1
																* d = cos (alpha);					  0				  -1			     0
																* e = coord X Rotation 	     ValHauteur       ValLargeur                 0	
																* f = coord Y Rotation			      0       ValHauteur        ValLargeur						
																* 
																* e et f = point de rotation
																* https://en.wikipedia.org/wiki/Rotation_matrix#Common_rotations
																*/
																if (vecRotation[i] == 90)
																	painter.SetTransformationMatrix(0.0, 1.0, -1.0, 0.0, (double)TamponHauteur, 0.0);
																else if (vecRotation[i] == 180)
																	painter.SetTransformationMatrix(-1.0, 0.0, 0.0, -1.0, (double)TamponLargeur, (double)TamponHauteur);
																else if (vecRotation[i] == 270)
																	painter.SetTransformationMatrix(0.0, -1.0, 1.0, 0.0, 0.0, (double)TamponLargeur);

																//  PdfImage image(&document);//Logo EDF PDG ?
																//  image.LoadFromJpeg("C:\\PODOFO Build\\image.jpg");
																//  painter.DrawImage(0.0, 0.0, &image);



																painter.SetStrokeWidth(TamponEpaisseur);
																//Fond du tampon en blanc avec bord de couleur rouge
																painter.SetStrokingColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);//Couleur ligne format RGB avec 0 à 255 = 0.0 à 1.0
																painter.SetColor(1.0, 1.0, 1.0);//Fond du tampon
																painter.Rectangle(TamponEpaisseur / 2, TamponEpaisseur / 2, TamponLargeur - TamponEpaisseur, TamponHauteur - TamponEpaisseur);
																painter.FillAndStroke();

																//Les lignes internes
																painter.DrawLine(0.0, TamponH1, TamponLargeur, TamponH1);
																painter.DrawLine(0.0, TamponH2, TamponLargeur, TamponH2);
																painter.DrawLine(TamponS1, TamponH1, TamponS1, TamponH2);
																painter.DrawLine(TamponS2, 0.0, TamponS2, TamponH1);

																painter.SetFont(pFont);//Utilise pFont pour écrire...

																painter.SetColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);//Couleur texte format RGB avec 0 à 255 = 0.0 à 1.0
																PoDoFo::PdfString utf8SiteDe(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Site de " + string(NomSite)).c_str()));
																PoDoFo::PdfString utf8Tranche(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Tr. : " + to_string(t)).c_str()));
																PoDoFo::PdfString utf8REE(reinterpret_cast<const PoDoFo::pdf_utf8*>(string(strREFERENCEREE).c_str()));
																PoDoFo::PdfString utf8Indice(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Ind. : " + string(strINDICEREE)).c_str()));
																PoDoFo::PdfString utf8Folio(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Folio : " + to_string(u16_PremierNumero + i - (radioTotalPartiel == 1 ? u16_PageDebut - 1 : 0))).c_str()));
																PoDoFo::PdfString utf8Cycle(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Cycle : " + to_string(t) + string(TrancheCode[t])).c_str()));

																painter.DrawTextAligned(TamponMargL, TamponMargH, TamponS2 - 2 * TamponMargL, utf8Folio, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
																painter.DrawTextAligned(TamponS2 + TamponMargL, TamponMargH, (TamponLargeur - TamponS2) - 2 * TamponMargL, utf8Cycle, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
																painter.DrawTextAligned(TamponMargL, TamponH1 + TamponMargH, TamponS1 - 2 * TamponMargL, utf8REE, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
																painter.DrawTextAligned(TamponS1 + TamponMargL, TamponH1 + TamponMargH, (TamponLargeur - TamponS1) - 2 * TamponMargL, utf8Indice, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
																painter.DrawTextAligned(TamponMargL, TamponH2 + TamponMargH, TamponLargeur - 2 * TamponMargL, utf8SiteDe, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
																painter.DrawTextAligned(TamponMargL, TamponH2 + TamponMargH, TamponLargeur - 2 * TamponMargL, utf8Tranche, PoDoFo::EPdfAlignment::ePdfAlignment_Right);//OK
																painter.FinishPage();


																/*
																* Affinage du Rect
																*   0,vecHeight
																*   X**************X vecWidth,vecHeight
																*   *              *
																*   *              *
																*   *              *
																*   *              *
																*   *              *
																*   *              *
																*   *              *
																*   *              *
																*   X**************X 0,vecwidth
																*  0,0
																*/

																if (radioEmplacementTampon == 0)
																{//Haut Gauche
																	rect.SetLeft(0.0 + (double)margeEmplacementTamponX);
																	rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponY);
																	if (vecRotation[i] == 0)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponX);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 90)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponX);
																		rect.SetBottom((double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 180)
																	{
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponX);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 270)
																	{
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponY);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponX);
																	}

																}
																else if (radioEmplacementTampon == 1)
																{//Haut Droite
																	if (vecRotation[i] == 0)
																	{
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponX);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 90)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponY);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponX);
																	}
																	else if (vecRotation[i] == 180)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponX);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 270)
																	{
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponY);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponX);
																	}
																}
																else if (radioEmplacementTampon == 2)
																{//Bas Gauche
																	if (vecRotation[i] == 0)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponX);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 90)
																	{//A Voir
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponY);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponX);
																	}
																	else if (vecRotation[i] == 180)
																	{//A Voir
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponX);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 270)
																	{//A voir
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponY);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponX);
																	}
																}
																else if (radioEmplacementTampon == 3)
																{//Bas Droite
																	if (vecRotation[i] == 0)
																	{
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponX);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 90)
																	{
																		rect.SetLeft(vecMediaBox[i].GetWidth() - rect.GetWidth() - (double)margeEmplacementTamponY);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponX);
																	}
																	else if (vecRotation[i] == 180)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponX);
																		rect.SetBottom(vecMediaBox[i].GetHeight() - rect.GetHeight() - (double)margeEmplacementTamponY);
																	}
																	else if (vecRotation[i] == 270)
																	{
																		rect.SetLeft(0.0 + (double)margeEmplacementTamponY);
																		rect.SetBottom(0.0 + (double)margeEmplacementTamponX);
																	}
																}

																PoDoFo::PdfAnnotation* pAnnotation = pPage->CreateAnnotation(PoDoFo::EPdfAnnotation::ePdfAnnotation_Stamp, rect);
																pAnnotation->SetFlags(PoDoFo::ePdfAnnotationFlags_Print);
																pAnnotation->SetTitle(PoDoFo::PdfString(string("Tampon_p" + to_string(i + 1))));
																pAnnotation->SetAppearanceStream(&xObj);


																painter.FinishPage();

															}
															MessageOPENCours = fmt::format(u8"Création des tampons {}\nSauvegarde du fichier PDF temporaire", string(txtSpinner));
															MessagePercentOPENCours = fmt::format("Tr.{}", t);
															document.GetInfo()->SetCreator(PoDoFo::PdfString("REEMaker 5"));
															document.GetInfo()->SetAuthor(reinterpret_cast<const PoDoFo::pdf_utf8*>(u8"Grégory WENTZEL"));
															document.GetInfo()->SetTitle(PoDoFo::PdfString("tampon foliotage REE"));
															document.GetInfo()->SetSubject(PoDoFo::PdfString("PDF vierge avec tampon"));
															document.Close();
														}
														MessageOPENCours = fmt::format(u8"Création des tampons {}\nFusion du fichier PDF temporaire avec la procédure", string(txtSpinner));
														MessagePercentOPENCours = fmt::format("Tr.{}", t);
														wstring localwCheminPDF = wCheminPDF;

														{//Partiel ==?? On ampute le PDF!!
															localwCheminPDF = TEMPTampon + to_wstring(t);
															SHELLEXECUTEINFO ShExecInfo;
															ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
															ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
															ShExecInfo.hwnd = NULL;
															ShExecInfo.lpVerb = NULL;
															ShExecInfo.lpFile = CheminPDFTK.c_str();
															wstring mParameter = L"\"" + wCheminPDF + L"\" cat " + to_wstring(u16_PageDebut) + L"-" + to_wstring(u16_PageFin) + L" output \"" + localwCheminPDF + L"\" need_appearances compress";
															ShExecInfo.lpParameters = mParameter.c_str();
															ShExecInfo.lpDirectory = CheminBASE.c_str();
															ShExecInfo.nShow = SW_HIDE;
															ShExecInfo.hInstApp = NULL;
															ShellExecuteEx(&ShExecInfo);
															WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
															CloseHandle(ShExecInfo.hProcess);
														}

														//Fusion avec PDFTK
														{
															SHELLEXECUTEINFO ShExecInfo;
															ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
															ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
															ShExecInfo.hwnd = NULL;
															ShExecInfo.lpVerb = NULL;
															ShExecInfo.lpFile = CheminPDFTK.c_str();
															OutputDebugStringW(FichierPDFsortie.c_str());
															OutputDebugStringW(L"\n");
															wstring mParameter;
															if (radioTotalPartiel == 1)
																mParameter = L"\"" + TEMPTampon + L"\" multistamp \"" + localwCheminPDF + L"\" output \"" + FichierPDFsortie.substr(0, FichierPDFsortie.size() - 4) + L"_TR" + to_wstring(t) + L".pdf\" need_appearances compress";
															else
																mParameter = L"\"" + TEMPTampon + L"\" multistamp \"" + wCheminPDF + L"\" output \"" + FichierPDFsortie.substr(0, FichierPDFsortie.size() - 4) + L"_TR" + to_wstring(t) + L".pdf\" need_appearances compress";
															ShExecInfo.lpParameters = mParameter.c_str();
															ShExecInfo.lpDirectory = CheminBASE.c_str();
															ShExecInfo.nShow = SW_HIDE;
															ShExecInfo.hInstApp = NULL;
															ShellExecuteEx(&ShExecInfo);
															WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
															CloseHandle(ShExecInfo.hProcess);
														}
														//Suppression partiel
														if (radioTotalPartiel == 1)
															filesystem::remove(localwCheminPDF);
														//Suppression temp PDF
														filesystem::remove(TEMPTampon);


													}
													catch (const PoDoFo::PdfError& e)
													{
														OutputDebugStringA("Erreur type PDFError : ");
														OutputDebugStringA(e.what());
														OutputDebugStringA("\n");
													}
													catch (exception& ex)
													{
														OutputDebugStringA("Erreur type exception : ");
														OutputDebugStringA(ex.what());
														OutputDebugStringA("\n");
													}
													catch (...)
													{
														OutputDebugStringA("Erreur type inconnu\n");
													}
												}
												MessageOPENCours = fmt::format(u8"Fin de la création des tampons.");
												MessagePercentOPENCours = fmt::format("");
												this_thread::sleep_for(1s);
												AfficheFenetreSpinnerthreadFolioSansPDG = false;
											});
										t.detach();

									}







								}
								ImGui::PopStyleVar();
								ImGui::SetNextWindowSize(ImVec2(OpEnCoursWidth, OpEnCoursHeight));
								if (ImGui::BeginPopupModal("Avancement##threadFolioSansPDG", NULL, ImGuiWindowFlags_NoResize))
								{
									ImGui::Text("");
									ImGui::SetCursorPosX((OpEnCoursWidth - 2 * 32.0f) / 2.0f);
									ImGui::LoadingIndicatorCircle("##LICAnalysePDF", 32.0f, IMVEC4_COL16(50, 50, 50, 255), IMVEC4_COL16(50, 50, 50, 140), 8, 10.0f);
									ImGui::SameLine();
									ImGui::PushFont(MYFont20);
									ImVec2 percentSize = ImGui::CalcTextSize(MessagePercentOPENCours.c_str());
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 32.0f - ((2.0f / 3.0f) * percentSize.y));
									ImGui::SetCursorPosX((OpEnCoursWidth - percentSize.x) / 2.0f);
									ImGui::TextUnformatted(MessagePercentOPENCours.c_str());
									ImGui::PopFont();

									ImGui::TextWrapped(MessageOPENCours.c_str());

									if (AfficheFenetreSpinnerthreadFolioSansPDG == false)
										ImGui::CloseCurrentPopup();
									ImGui::EndPopup();
								}


								ImGui::EndTable();
							}
					}

					ImGui::SetNextWindowSize(ImVec2(OpEnCoursWidth, OpEnCoursHeight));
					if (ImGui::BeginPopupModal("Avancement##TABfolioteruneprocedure", NULL, ImGuiWindowFlags_NoResize))
					{
						ImGui::Text("");
						ImGui::SetCursorPosX((OpEnCoursWidth - 2 * 32.0f) / 2.0f);
						ImGui::LoadingIndicatorCircle("##LICAnalysePDF", 32.0f, IMVEC4_COL16(50, 50, 50, 255), IMVEC4_COL16(50, 50, 50, 140), 8, 10.0f);
						ImGui::SameLine();
						ImGui::PushFont(MYFont20);
						ImVec2 percentSize = ImGui::CalcTextSize(MessagePercentOPENCours.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 32.0f - ((2.0f / 3.0f) * percentSize.y));
						ImGui::SetCursorPosX((OpEnCoursWidth - percentSize.x) / 2.0f);
						ImGui::TextUnformatted(MessagePercentOPENCours.c_str());
						ImGui::PopFont();

						ImGui::TextWrapped(MessageOPENCours.c_str());

						if (AfficheFenetreSpinner == false)
							ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_FA_FILE_TEXT_O " Page de garde"))
				{
					ImGui::Text("Onglet n°2 : " ICON_FA_BLUETOOTH " " ICON_FA_AMAZON " " ICON_FA_ARROW_LEFT " " ICON_FA_CIRCLE_THIN " " ICON_FA_CLIPBOARD " " ICON_FA_CODIEPIE);


					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_FA_COGS " Options"))
				{
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Couleurs prédéfinies :");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(u8"Couleurs prédéfinies :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Ici, vous pouvez retrouver la liste des couleurs disponibles pour les tampons / pages de gardes."); ImGui::Text("");

					if (ImGui::BeginTable("##TableCouleurGaucheTitre", 1, ImGuiTableFlags_ScrollY, ImVec2(400.0f, 40.0f)))
					{
						static int PressedSelectable = -1;
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::PushFont(MYFont14bold);
						ImGui::Text(u8"Liste des couleurs prédéfinies :");
						ImGui::PopFont();


						ImGui::EndTable();
					}
					ImGui::SameLine();
					if (ImGui::BeginTable("##TableCouleurDroiteTitre", 1, ImGuiTableFlags_ScrollY, ImVec2(-1.0f, 40.0f)))
					{
						static int PressedSelectable = -1;
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::PushFont(MYFont14bold);
						ImGui::Text(u8"Ajouter une couleur à la liste des couleurs prédéfinies :");
						ImGui::PopFont();
						ImGui::SameLine(); HelpMarker(u8"Ici, vous allez pouvoir ajouter une nouvelle couleur à la liste des couleurs prédéfinies."); ImGui::Text("");


						ImGui::EndTable();
					}
					if (ImGui::BeginTable("##TableCouleurGauche", 1, ImGuiTableFlags_ScrollY, ImVec2(400.0f, 120.0f)))
					{
						static int PressedSelectable = -1;
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						for (size_t i = 0; i < ListeCouleur.size(); i++)
						{
							if (ImGui::Selectable(string(ListeCouleur[i].NomCouleur + "##CB" + to_string(i) + ListeCouleur[i].NomCouleur).c_str(), false/*&ListeCouleurSelectable[i]*/, ImGuiSelectableFlags_None, ImVec2(200.0f, 20.0f)))
							{
								PressedSelectable = i;
								ImGui::OpenPopup("##my_select_popup");
							}
							ImGui::SameLine();
							ImGui::ColorButton(string(ListeCouleur[i].NomCouleur + "##CB" + to_string(i) + ListeCouleur[i].NomCouleur).c_str(), ListeCouleur[i].CodeCouleur, ImGuiColorEditFlags_NoAlpha | (ListeCouleur[i].Locked ? ImGuiColorEditFlags_NoPicker : 0));
						}
						if (ImGui::BeginPopup("##my_select_popup"))
						{
							ImGui::PushFont(MYFont14bold);
							ImGui::Text("Action");
							ImGui::PopFont();
							ImGui::Separator();
							if (ListeCouleur[PressedSelectable].Locked)
								ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), string(u8"Supprimer la couleur '" + ListeCouleur[PressedSelectable].NomCouleur + "'").c_str());
							else
								if (ImGui::Selectable(string(u8"Supprimer la couleur '" + ListeCouleur[PressedSelectable].NomCouleur + "'").c_str()))
								{
									ListeCouleur.erase(ListeCouleur.begin() + PressedSelectable);
									OutputDebugStringA("Suppression de ");
									OutputDebugStringA(to_string(PressedSelectable).c_str());
									OutputDebugStringA("\n");
								}
							if (ImGui::Selectable(u8"Annuler"))
							{
								OutputDebugStringA("Annuler");
							}
							ImGui::EndPopup();
						}
						ImGui::EndTable();
					}
					ImGui::SameLine();
					if (ImGui::BeginTable("##TableCouleurDroite", 1, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, 120.0f)))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						static ImVec4 TheNewColor = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);
						ImGui::Text(u8"Réglage de la nouvelle couleur :");
						ImGui::ColorEdit3("Nouvelle couleur##1", (float*)&TheNewColor, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha);
						ImGui::Text(u8"Nom de cette couleur :");
						static char NomDeLaCouleur[32] = "";
						if (string(NomDeLaCouleur).size() > 0)
						{
							ImGui::PushFont(MYFont14bold);
							ImGui::InputTextWithHint("##NouvelleCouleurNom", "ex. Rose fuchsia", NomDeLaCouleur, IM_ARRAYSIZE(NomDeLaCouleur));
							ImGui::PopFont();
						}
						else
							ImGui::InputTextWithHint("##NouvelleCouleurNom", "ex. Rose fuchsia", NomDeLaCouleur, IM_ARRAYSIZE(NomDeLaCouleur));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						if (ImGui::Button(u8"Ajouter cette couleur à la liste des couleurs prédéfinies") && string(NomDeLaCouleur).size() > 0)
						{
							mCouleur.NomCouleur = string(NomDeLaCouleur);
							mCouleur.Locked = false;
							mCouleur.CodeCouleur = TheNewColor;
							ListeCouleur.push_back(mCouleur);


							NomDeLaCouleur[0] = '\0';
						}
						ImGui::PopStyleVar();


						ImGui::EndTable();
					}

					ImGui::Separator();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Définition des couleurs à utiliser pour chacune des tranches :");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(
						u8"Définition des couleurs à utiliser pour chacune des tranches :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Vous pouvez faire glisser une couleur de la liste des couleurs prédéfinies dans la tranche souhaitée."); ImGui::Text("");
					for (size_t k = 0; k < 10; k++)
					{
						ImGui::Text("Couleur du tampon tranche %d :", k);
						ImGui::SameLine();
						ImGui::ColorEdit4(string("##CouleurTranche" + to_string(k)).c_str(), sListeCouleurTranche[k], ImGuiColorEditFlags_NoAlpha);
					}
					ImGui::Separator();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Emplacement du tampon et marge:");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(
						u8"Emplacement du tampon et marge:").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Vous allez sélectionner l'emplacement sur la page ou le tampon sera apposé."); ImGui::Text("");
					ImGui::Text(u8"Emplacement du tampon et marge :"); ImGui::SameLine();
					ImGui::RadioButton(u8"En haut à gauche", &radioEmplacementTampon, 0); ImGui::SameLine();
					ImGui::RadioButton(u8"En haut à droite", &radioEmplacementTampon, 1); ImGui::SameLine();
					ImGui::RadioButton(u8"En bas à gauche", &radioEmplacementTampon, 2); ImGui::SameLine();
					ImGui::RadioButton(u8"En bas à droite", &radioEmplacementTampon, 3);
					ImGui::Text(u8"Marge du tampon :"); ImGui::SameLine(); ImGui::Text(u8"Largeur en point : "); ImGui::SameLine();
					ImGui::InputScalar("##ScalarMargeX", ImGuiDataType_U16, &margeEmplacementTamponX, &u16_one, &u16_fast, "%u");
					ImGui::SameLine(); ImGui::Text(u8"Hauteur en point : "); ImGui::SameLine();
					ImGui::InputScalar("##ScalarMargeY", ImGuiDataType_U16, &margeEmplacementTamponY, &u16_one, &u16_fast, "%u");

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_FA_INFO " A propos"))
				{
					const std::string markdownText = u8R"(
### REEMaker
Version 5.0, développé en C++ 17 sous Visual Studio 2019
Par Grégory WENTZEL [gregory.wentzel@edf.fr](mailto:gregory.wentzel@edf.fr)
Programme sous licence GPL 2

## Les librairies suivantes sont utilisés :
  * [Simple Directmedia Layer : SDL 2](https://www.libsdl.org/index.php)
    Librairie de rendu graphique OpenGL 2/3.
    [Licence 'Zlib licence'](https://www.zlib.net/zlib_license.html).
  * [Dear IMGui](https://github.com/ocornut/imgui)
    Librairie d'interface utilisateur C++.
    [Licence 'MIT'](https://raw.githubusercontent.com/ocornut/imgui/master/LICENSE.txt).
  * [Wintoast](https://github.com/mohabouje/WinToast)
    Librairie pour gestion de notification Windows.
    [Licence MIT](https://raw.githubusercontent.com/mohabouje/WinToast/master/LICENSE.txt).
  * [PoDoFo Version 0.96](http://podofo.sourceforge.net)
    Librairie de lecture /écriture de fichier PDF.
    [Licence LGPL](http://www.gnu.org/copyleft/lesser.html).
  * [GNU LibICONV / GNU LibCharset](https://www.gnu.org/software/libiconv)
    Gestion flux utf8 / Unicode.
    [Licence LGPL](https://en.wikipedia.org/wiki/LGPL).
  * [LybCrypto et LibSSL {OpenSSL}](https://www.openssl.org/)
    Librairie pour les fichiers PDF cryptés.
    [Licence Apache 2.0](https://www.openssl.org/source/license.html).
  * [LibIDN 2](https://gitlab.com/libidn/libidn2)
    Gestion flux utf8 / Unicode.
    [Licence LGPL 3](https://gitlab.com/libidn/libidn2/-/raw/master/COPYING.LESSERv3).
  * [XXHash](https://github.com/Cyan4973/xxHash)
    Librairie de Hashing.
    [Licence 'BSD 2-Clause License'](https://raw.githubusercontent.com/Cyan4973/xxHash/dev/LICENSE).

  * [Google Brotli](https://github.com/google/brotli/)
    Librairie de compression/décompression.
    [Licence MIT](https://raw.githubusercontent.com/google/brotli/master/LICENSE).
  * [BZip 2](http://www.bzip.org/)
    Librairie de compression/décompression.
    [Licence BSD-like](https://en.wikipedia.org/wiki/BSD-like_license).
  * [LZMA](http://www.7-zip.org/sdk.html)
    Librairie de compression/décompression.
    [domaine public](http://www.7-zip.org/sdk.html).
  * [Zlib](https://www.zlib.net/)
    Librairie de compression/décompression.
    [Licence Zlib](https://www.zlib.net/zlib_license.html).
  * [Zstd](https://github.com/facebook/zstd)
    Librairie de compression/décompression.
    [Licence BSD](https://raw.githubusercontent.com/facebook/zstd/dev/LICENSE).

  * [Freetype](https://www.freetype.org/)
    Librairie de rendu de police d'écran.
    [Licence 'Freetype Licence'](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/FTL.TXT).
  * [LibJPEG](http://libjpeg.sourceforge.net/)
    Gestion de fichier Jpeg.
    [Licence BSD-Like](https://en.wikipedia.org/wiki/Libjpeg).
  * [TurboJPEG](https://github.com/libjpeg-turbo/libjpeg-turbo)
    Gestion de fichier Jpeg.
    [Licence Zlib](https://opensource.org/licenses/Zlib).
  * [LibPNG16](http://www.libpng.org/pub/png/libpng.html)
    Gestion de fichier PNG Portable Network Graphics.
    [Licence Open Source](http://www.libpng.org/pub/png/src/libpng-LICENSE.txt).
  * [Tiff](https://gitlab.com/libtiff/libtiff)
    Gestion de fichier Tiff.
    Licence domaine public.

## Les executables suivants sont utilisés :
  * [PDFToolkit Server](https://www.pdflabs.com/tools/pdftk-server)
    Manipulation de fichier PDF.
    [Licence 'GPL v2'](https://www.pdflabs.com/docs/pdftk-license/gnu_general_public_license_2.txt).

## Les codes suivants sont utilisés :
  * [imgui_markdown](https://github.com/juliettef/imgui_markdown/)
    Librairie d'affichage de texte au format Markdown.
    [Licence 'zlib License'](https://raw.githubusercontent.com/juliettef/imgui_markdown/master/License.txt/).
  * [Obfuscate](https://github.com/adamyaxley/Obfuscate/)
    Obfuscation de code à la compilation.
    [Licence 'The Unlicense'](https://raw.githubusercontent.com/adamyaxley/Obfuscate/master/LICENSE/).
  * [PLF Nanotimer](https://github.com/mattreecebentley/plf_nanotimer/)
    Timer mesurant les nanosecondes.
    [Licence 'Zlib Licence'](https://en.wikipedia.org/wiki/Zlib_License/).

## Les polices suivantes sont utilisées :
  * [Droid Sans par Steve Matteson](https://en.wikipedia.org/wiki/Steve_Matteson)
    [Licence 'Apache License'](https://en.wikipedia.org/wiki/Apache_License).

## Les icones suivants sont utilisés :
  * [Stamp Icon par DesignContest](http://www.designcontest.com/)
    [Licence 'CC Attribution 4.0'](https://creativecommons.org/licenses/by/4.0/).
)";
					static ImGui::MarkdownConfig mdConfig;
					mdConfig.linkCallback = [](ImGui::MarkdownLinkCallbackData data_)
					{
						std::string url(data_.link, data_.linkLength);
						if (!data_.isImage)
						{
							ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
						}
					};
					mdConfig.linkIcon = ICON_FA_LINK;
					mdConfig.headingFormats[0] = { MYFont14bold, false/*Sousligné*/ };//H1 #
					mdConfig.headingFormats[1] = { MYFont14bold, true/*Sousligné*/ }; //H2 ##
					mdConfig.headingFormats[2] = { MYFont20, false/*Sousligné*/ };    //H3 ###
					mdConfig.userData = NULL;
					if (ImGui::BeginTable("##TableMarkdown", 1, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_ScrollY, ImVec2(-1.0f, height - 2 * ImGui::GetFrameHeightWithSpacing())))
					{

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						auto SAVColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(SAVColor.x - 0.2f, SAVColor.y - 0.2f, SAVColor.z - 0.2f, 1.0f));
						ImGui::Markdown(markdownText.c_str(), markdownText.length(), mdConfig);
						ImGui::PopStyleColor();

						ImGui::EndTable();
					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			//if (ImGuiAl::Button("Compresser le fichier 'compressible'", !AfficheFenetreSpinner))
			//{
			//	ImGui::OpenPopup("Avancement##OpEnCours");
			//	AfficheFenetreSpinner = true;
			//	static float lvlCMP2 = 0;
			//	lvlCMP2 += 1.0f;
			//	lvlCMP2 += 1.0f;
			//	lvlCMP2 += 1.0f;
			//	lvlCMP2 += 1.0f;
			//	lvlCMP2 += 1.0f;
			//	lvlCMP2 += 1.0f;
			//	lvlCMP2 += 1.0f;
			//	float lvlCMP = 0;
			//	std::thread t([&key, &iv, &Chemin, &lvlCMP]()
			//		{
			//		std::cout << "thread function\n";
			//		FILE* inFile = NULL;
			//		MessageOPENCours = "Lecture du contenu du fichier source...";
			//		std::string File2open = /*"C:\\MDP TeamViewer\\NUKLEAR_GDIP"*/ Chemin;
			//		long long szFile2open = std::filesystem::file_size(File2open);
			//		std::vector<unsigned char> BufferIN;
			//		BufferIN.resize(szFile2open);
			//		fopen_s(&inFile, File2open.c_str(), "rb");
			//		size_t const szRead = fread_s(&BufferIN[0], BufferIN.size(), sizeof(unsigned char), BufferIN.size(), inFile);
			//		fclose(inFile);
			//		if (szRead != BufferIN.size()) {
			//			printf("Erreur lecture");
			//		}
			//		//LEVEL de -5 à 22
			//		// ON TESTE DE 0 à 22
			//		while(true)
			//		{
			//			MessageOPENCours = std::string("Compression en cours niveau " + std::to_string((int)lvlCMP) + "\nThis text should automatically wrap on the edge of the window. The current implementation This text should automatically wrap on the edge of the window. The current implementation This text should automatically wrap on the edge of the window. The current implementation");
			//			MessagePercentOPENCours = std::string(std::to_string((int)((100.0f / 22.0f) * lvlCMP))) + " %";
			//			OutputDebugStringA("\n");
			//			printf("--------------------------------------------------------------\n");
			//			printf("Niveau de compression : %Id\n", lvlCMP);
			//			std::vector<unsigned char> chunkCompAndCrypt;
			//			plf::nanotimer mTimer;
			//			mTimer.start();
			//			mesErreur result = CompressAndCryptChunk(BufferIN, chunkCompAndCrypt, (int)lvlCMP, key, iv);
			//			double data_MB = chunkCompAndCrypt.size();
			//			data_MB = data_MB / (double)1024 / (double)1024;
			//			double tElapsed = (double)mTimer.get_elapsed_ms();
			//			double data_rate_MBps = data_MB / (tElapsed / 1000.);
			//			sLock.lock();
			//			BenchStruct mBStruct;
			//			mBStruct.Level = (int)lvlCMP;
			//			mBStruct.TailleOriginal = BufferIN.size();
			//			mBStruct.TailleCompressé = chunkCompAndCrypt.size();
			//			mBStruct.Ratio = (double)chunkCompAndCrypt.size() / (double)BufferIN.size();
			//			mBStruct.TempsCompression = tElapsed;
			//			mBStruct.DebitMbs = data_rate_MBps;
			//			vecBench.push_back(mBStruct);
			//			sLock.unlock();
			//			//On simule une lecture
			//			//Lecture 4 premier bytes
			//			uint32_t compSize = MEM_read32(&chunkCompAndCrypt[0]);
			//			//Lecture 4  bytes suivants
			//			uint32_t DecompSize = MEM_read32(&chunkCompAndCrypt[4]);
			//			//Lecture 8  bytes suivants
			//			uint64_t uint64XXH3 = MEM_read64(&chunkCompAndCrypt[8]);
			//			//On y va
			//			//On simule la lecture du fichier de la taille de compsize
			//			std::vector<unsigned char> BufferDecomp;
			//			std::vector<unsigned char> BufferLU;
			//			BufferLU.resize(chunkCompAndCrypt.size() - HeaderSize);
			//			memcpy_s(&BufferLU[0], compSize, &chunkCompAndCrypt[HeaderSize], compSize);
			//			//Fin simulation lecture
			//			result = DecompressCryptChunk(BufferLU, BufferDecomp, compSize, DecompSize, uint64XXH3, key, iv);
			//			lvlCMP += 1.0f;
			//			if (lvlCMP >= 23.0f)
			//				break; 
			//		}
			//		MessageOPENCours = fmt::format("Fin des opérations...");
			//		AfficheFenetreSpinner = false;
			//		});
			//	t.detach();
			//}
			//if (AfficheFenetreSpinner)
			//{
			//	ImGui::SameLine();
			//	ImGui::LoadingIndicatorCircle("##Lic", 8.0f, IMVEC4_COL16(50, 50, 50, 255), IMVEC4_COL16(50, 50, 50, 140), 8, 10.0f);
			//}
			//sLock.lock();
			//for (size_t i = 0; i < vecBench.size(); i++)
			//{
			//	std::string fmtText = fmt::format("Niveau {}    {:03.2f}% ({:06.2f}/{:06.2f}Ko) en {:06.2f}ms [{:03.2f}Mo/s]",
			//		vecBench[i].Level, vecBench[i].Ratio * 100, vecBench[i].TailleCompressé / 1024., vecBench[i].TailleOriginal / 1024.,
			//		vecBench[i].TempsCompression, vecBench[i].DebitMbs);
			//	ImGui::ProgressBar(vecBench[i].Ratio, ImVec2(600.0f, 15.0f), fmtText.c_str());
			//}
			//sLock.unlock();
			//ImGui::SetNextWindowSize(ImVec2(OpEnCoursWidth, OpEnCoursHeight));
			//if (ImGui::BeginPopupModal("Avancement##OpEnCours", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			//{
			//	ImGui::Text("");
			//	ImGui::SetCursorPosX((OpEnCoursWidth - 2 * 32.0f) / 2.0f);
			//	ImGui::LoadingIndicatorCircle("##LICopencours", 32.0f, IMVEC4_COL16(50, 50, 50, 255), IMVEC4_COL16(50, 50, 50, 140), 8, 10.0f);
			//	ImGui::SameLine();
			//	ImGui::PushFont(MYFont20);
			//	ImVec2 percentSize = ImGui::CalcTextSize(MessagePercentOPENCours.c_str());
			//	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 32.0f - ((2.0f / 3.0f) * percentSize.y));
			//	ImGui::SetCursorPosX((OpEnCoursWidth - percentSize.x) / 2.0f);
			//	ImGui::TextUnformatted(MessagePercentOPENCours.c_str());
			//	ImGui::PopFont();
			//	ImGui::TextWrapped(MessageOPENCours.c_str());
			//	if (!AfficheFenetreSpinner)
			//		ImGui::CloseCurrentPopup();
			//	ImGui::EndPopup();
			//}
			/*
			* Affichage Mode sombre / Clair
			* + FPS
			*/
			ImGui::PushFont(MYFont10bold);
			ImGui::SetCursorPosX(width - ImGui::CalcTextSize("(60.0 FPS)").x - ImGui::CalcTextSize("Thème sombre").x - /*Bordure*/24.0f - /*Checkbox*/24.0f - /*Espace entre widget*/12.0f);
			ImGui::SetCursorPosY(6.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::Checkbox("##ThemeSombre", &ThemeSombre);
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::Text(u8"Thème sombre");
			ImGui::SameLine();
			ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
			ImGui::PopFont();
#ifdef DEBUG
			ImGui::SameLine();
			ImGui::SetCursorPosX(700.0f);
			ImGui::PushFont(MYFont10bold);
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::PopFont();
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
#endif // DEBUG
			if (ThemeSombre)
				ImGui::StyleColorsDarkCharcoal();
			else
				ImGui::StyleColorBlackWhite();
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}
	SauveParametres();
	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool SauveParametres(/*bool _ThemeSombre, char* Tranche0, char* Tranche1, char* Tranche2, char* Tranche3, char* Tranche4, char* Tranche5, char* Tranche6, char* Tranche7, char* Tranche8, char* Tranche9, char* refREE, char* indREE*/)
{
	TCHAR path[4096];
	DWORD length;
	length = GetModuleFileName(NULL, path, 4096);
	/*
		Chemin de base de l'executable, fini par \
	*/
	wstring CheminBASE(path);
	CheminBASE = CheminBASE.substr(0, CheminBASE.find_last_of(L"\\") + 1);

	wstring CheminConfig = CheminBASE + L"REEMAKER.cfg";
	FILE* fCONFIG = NULL;
	if (_wfopen_s(&fCONFIG, CheminConfig.c_str(), L"wb") != 0)
		return false;
	//THEME
	FILE_write16(fCONFIG, (uint16_t)ThemeSombre);
	//Cycle des tranches
	fwrite(TrancheCode[0], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[1], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[2], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[3], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[4], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[5], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[6], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[7], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[8], sizeof(unsigned char), 6, fCONFIG);
	fwrite(TrancheCode[9], sizeof(unsigned char), 6, fCONFIG);
	//Référence et indice REE
	fwrite(strREFERENCEREE, sizeof(unsigned char), 32, fCONFIG);
	fwrite(strINDICEREE, sizeof(unsigned char), 5, fCONFIG);
	//Nombre de couleurs enregistrées
	FILE_write16(fCONFIG, (uint16_t)ListeCouleur.size() - 4);
	//Toutes les couleurs supplémentaires
	for (size_t z = 4; z < ListeCouleur.size(); z++)
	{
		static char NomDeLaCouleurSafz[64] = "";
		memcpy_s(NomDeLaCouleurSafz, 64, &ListeCouleur[z].NomCouleur[0], ListeCouleur[z].NomCouleur.size());
		fwrite(NomDeLaCouleurSafz, sizeof(unsigned char), 64, fCONFIG);
		FILE_write16(fCONFIG, (uint16_t)(ListeCouleur[z].CodeCouleur.x * 255));
		FILE_write16(fCONFIG, (uint16_t)(ListeCouleur[z].CodeCouleur.y * 255));
		FILE_write16(fCONFIG, (uint16_t)(ListeCouleur[z].CodeCouleur.z * 255));
		FILE_write16(fCONFIG, (uint16_t)(ListeCouleur[z].CodeCouleur.w * 255));
	}
	//Couleurs des tranches
	for (size_t i = 0; i < 10; i++)
	{
		FILE_write16(fCONFIG, (uint16_t)(sListeCouleurTranche[i][0] * 255));
		FILE_write16(fCONFIG, (uint16_t)(sListeCouleurTranche[i][1] * 255));
		FILE_write16(fCONFIG, (uint16_t)(sListeCouleurTranche[i][2] * 255));
		FILE_write16(fCONFIG, (uint16_t)(sListeCouleurTranche[i][3] * 255));
	}
	//Emplacement Tampon
	FILE_write16(fCONFIG, (uint16_t)radioEmplacementTampon);

	//Marge X et Y
	FILE_write16(fCONFIG, (uint16_t)margeEmplacementTamponX);
	FILE_write16(fCONFIG, (uint16_t)margeEmplacementTamponY);

	//Nom site
	fwrite(NomSite, sizeof(unsigned char), 32, fCONFIG);
	fclose(fCONFIG);
	printf("");
	return true;
}

bool ChargeParametres(/*bool& _ThemeSombre, char* Tranche0, char* Tranche1, char* Tranche2, char* Tranche3, char* Tranche4, char* Tranche5, char* Tranche6, char* Tranche7, char* Tranche8, char* Tranche9, char* refREE, char* indREE*/)
{
	TCHAR path[4096];
	DWORD length;
	length = GetModuleFileName(NULL, path, 4096);
	/*
		Chemin de base de l'executable, fini par \
	*/
	wstring CheminBASE(path);
	CheminBASE = CheminBASE.substr(0, CheminBASE.find_last_of(L"\\") + 1);

	wstring CheminConfig = CheminBASE + L"REEMAKER.cfg";
	FILE* fCONFIG = NULL;
	if (_wfopen_s(&fCONFIG, CheminConfig.c_str(), L"rb") != 0)
		return false;
	//THEME
	ThemeSombre = (bool)FILE_read16(fCONFIG);
	//Cycle des tranches
	fread_s(TrancheCode[0], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[1], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[2], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[3], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[4], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[5], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[6], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[7], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[8], 6, sizeof(unsigned char), 6, fCONFIG);
	fread_s(TrancheCode[9], 6, sizeof(unsigned char), 6, fCONFIG);
	//Référence et indice REE
	fread_s(strREFERENCEREE, 32, sizeof(unsigned char), 32, fCONFIG);
	fread_s(strINDICEREE, 5, sizeof(unsigned char), 5, fCONFIG);
	//Nombre de couleurs enregistrées
	int nbCouleur = FILE_read16(fCONFIG);
	//Toutes les couleurs supplémentaires
	for (size_t i = 0; i < nbCouleur; i++)
	{
		static char NomDeLaCouleurSafz[64] = "";
		fread_s(NomDeLaCouleurSafz, 64, sizeof(unsigned char), 64, fCONFIG);
		int CouleurX = FILE_read16(fCONFIG);
		int CouleurY = FILE_read16(fCONFIG);
		int CouleurZ = FILE_read16(fCONFIG);
		int CouleurW = FILE_read16(fCONFIG);
		Couleur _Couleur;
		_Couleur.Locked = false;
		_Couleur.NomCouleur = string(NomDeLaCouleurSafz);
		_Couleur.CodeCouleur = IMVEC4_COL16(CouleurX, CouleurY, CouleurZ, CouleurW);
		ListeCouleur.push_back(_Couleur);
	}
	//Couleurs des tranches
	for (size_t i = 0; i < 10; i++)
	{
		sListeCouleurTranche[i][0] = (float)(FILE_read16(fCONFIG) / 255.0f);
		sListeCouleurTranche[i][1] = (float)(FILE_read16(fCONFIG) / 255.0f);
		sListeCouleurTranche[i][2] = (float)(FILE_read16(fCONFIG) / 255.0f);
		sListeCouleurTranche[i][3] = (float)(FILE_read16(fCONFIG) / 255.0f);
	}
	//Emplacement Tampon
	radioEmplacementTampon = FILE_read16(fCONFIG);

	//Marge X et Y
	margeEmplacementTamponX = FILE_read16(fCONFIG);
	margeEmplacementTamponY = FILE_read16(fCONFIG);

	//Nom site
	fread_s(NomSite, 32, sizeof(unsigned char), 32, fCONFIG);

	fclose(fCONFIG);
	printf("");

	return true;
}
wstring generate(int max_length) {
	wstring possible_characters = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	random_device rd;
	mt19937 engine(rd());
	uniform_int_distribution<> dist((int)0, (int)(possible_characters.size() - 1));
	wstring ret = L"";
	for (int i = 0; i < max_length; i++) {
		int random_index = dist(engine); //get index between 0 and possible_characters.size()-1
		ret += possible_characters[random_index];
	}
	return ret;
}

/*
	https://www.codeproject.com/articles/16678/vista-goodies-in-c-using-the-new-vista-file-dialog
void CMainDlg::OnFileSave()
{
	HRESULT hr;
	CComPtr<IFileSaveDialog> pDlg;
	COMDLG_FILTERSPEC aFileTypes[] = {
		{ L"Text files", L"*.txt" },
		{ L"All files", L"*.*" }
	};

	// Create the file-save dialog COM object.
	hr = pDlg.CoCreateInstance(__uuidof(FileSaveDialog));

	if (FAILED(hr))
		return;

	// Set the dialog's caption text, file types, Save button label,
	// default file name, and default extension.
	// NOTE: Error handling omitted here for clarity.
	pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
	pDlg->SetTitle(L"A File-Save Dialog");
	pDlg->SetOkButtonLabel(L"D&o It!");
	pDlg->SetFileName(L"mystuff.txt");
	pDlg->SetDefaultExtension(L"txt");

	// Show the dialog.
	hr = pDlg->Show(m_hWnd);

	// If the user chose a file, save the user's data to that file.
	if (SUCCEEDED(hr))
	{
		CComPtr<IShellItem> pItem;

		hr = pDlg->GetResult(&pItem);

		if (SUCCEEDED(hr))
		{
			LPOLESTR pwsz = NULL;

			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

			if (SUCCEEDED(hr))
			{
				//TODO: Save the file here, 'pwsz' has the full path
				CoTaskMemFree(pwsz);
			}
		}
	}
}
void CMainDlg::OnFileOpen()
{
	HRESULT hr;
	CComPtr<IFileOpenDialog> pDlg;
	COMDLG_FILTERSPEC aFileTypes[] = {
		{ L"Text files", L"*.txt" },
		{ L"Executable files", L"*.exe;*.dll" },
		{ L"All files", L"*.*" }
	};

	// Create the file-open dialog COM object.
	hr = pDlg.CoCreateInstance(__uuidof(FileOpenDialog));

	if (FAILED(hr))
		return;

	// Set the dialog's caption text and the available file types.
	// NOTE: Error handling omitted here for clarity.
	pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
	pDlg->SetTitle(L"A Single-Selection Dialog");*
 Show the dialog.
		hr = pDlg->Show(m_hWnd);
	// If the user chose a file, show a message box with the
// full path to the file.
	if (SUCCEEDED(hr))
	{
		CComPtr<IShellItem> pItem;

		hr = pDlg->GetResult(&pItem);

		if (SUCCEEDED(hr))
		{
			LPOLESTR pwsz = NULL;

			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

			if (SUCCEEDED(hr))
			{
				MessageBox(pwsz);
				CoTaskMemFree(pwsz);
			}
		}
	}
}
void CMainDlg::MultipleFileOpen()

 CComPtr<IFileOpenDialog> pDlg;
  // Previous setup steps omitted...

DWORD dwFlags = 0;

pDlg->GetOptions(&dwFlags);
pDlg->SetOptions(dwFlags | FOS_ALLOWMULTISELECT);

// Show the dialog.
hr = pDlg->Show(m_hWnd);
 If the user chose any files, loop thru the array of files.
if (SUCCEEDED(hr))
{
	CComPtr<IShellItemArray> pItemArray;

	hr = pDlg->GetResults(&pItemArray);

	if (SUCCEEDED(hr))
	{
		DWORD cSelItems;

		// Get the number of selected files.
		hr = pItemArray->GetCount(&cSelItems);

		if (SUCCEEDED(hr))
		{
			for (DWORD j = 0; j < cSelItems; j++)
			{
				CComPtr<IShellItem> pItem;

				// Get an IShellItem interface on the next file.
				hr = pItemArray->GetItemAt(j, &pItem);

				if (SUCCEEDED(hr))
				{
					LPOLESTR pwsz = NULL;

					// Get its file system path.
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

					if (SUCCEEDED(hr))
					{
						MessageBox(pwsz);
						CoTaskMemFree(pwsz);
					}
				}
			}
		}
	}
}
*/