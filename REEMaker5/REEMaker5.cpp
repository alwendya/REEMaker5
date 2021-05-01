/*
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
* ░▒▓	 _____           __  __       _               _____		▓▒░
* ░▒▓	|  __ \         |  \/  |     | |             | ____|	▓▒░
* ░▒▓	| |__) |___  ___| \  / | __ _| | _____ _ __  | |__		▓▒░
* ░▒▓	|  _  // _ \/ _ \ |\/| |/ _` | |/ / _ \ '__| |___ \		▓▒░
* ░▒▓	| | \ \  __/  __/ |  | | (_| |   <  __/ |     ___) |	▓▒░
* ░▒▓	|_|  \_\___|\___|_|  |_|\__,_|_|\_\___|_|    |____/		▓▒░
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
*  REEMaker 5 __ Grégory WENTZEL (c) 2021
*/

#include "Header_Main.h"
#include "Header__Font.h"
#include "Header__Function.h"
#include "Header__FileHelper.h"
#include "Header__PdgHelper.h"
#include "Header__ImGUI_addon.h"
#include "Header__CryptoZstd.h"
#include "Header__WinToast.h"
#include "Header__FileWatcher.h"
using namespace std;

//#define EPR

// (っ◔◡◔)っ 𝙋𝙧𝙤𝙩𝙤𝙩𝙮𝙥𝙚 𝙛𝙤𝙣𝙘𝙩𝙞𝙤𝙣
bool SauveParametres();
bool ChargeParametres();
bool GenereMiniature(int, int, int, std::wstring);
bool GenereMiniatureMIN(int, int, int, std::wstring);
void MyTrace(int line, const char* fileName, const char* msg, ...);
void MyMsg(int line, const char* fileName, const char* msg, ...);
bool OuvrePDF(std::wstring);

#define MY_TRACE(msg, ...) MyTrace(__LINE__, __FILE__, msg"\n", __VA_ARGS__)
#define MY_MSG(msg, ...) MyMsg(__LINE__, __FILE__, msg"\n", __VA_ARGS__)

// (っ◔◡◔)っ 𝘾𝙡𝙖𝙨𝙨𝙚𝙨 𝙚𝙩 𝙎𝙩𝙧𝙪𝙘𝙩𝙪𝙧𝙚𝙨
class PDFError :public exception
{
public:
	const char* what() const throw()
	{
		return "Erreur de PDF";
	}
};
struct Couleur
{
	string NomCouleur = "";
	bool Locked = false;
	ImVec4 CodeCouleur = { 0.0f,0.0f,0.0f,0.0f };
};
struct TextureSTB
{
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;
	bool my_image_success_loading = false;
	std::string Chemin = "";
};

// (っ◔◡◔)っ 𝙑𝙖𝙧𝙞𝙖𝙗𝙡𝙚𝙨
FileHelper fileHELPER(L"");
PDGHelper mPDGHelper;
ImVec4 clear_color = IMVEC4_COL16(115, 135, 150, 255);
static ImVec2 BoutonSuivant(130.0f, 22.0f);
HWND mHwnd;
constexpr auto OpEnCoursWidth = 200.0f;
constexpr auto OpEnCoursHeight = 200.0f;
constexpr auto mFenWidth = 1280.0f;
constexpr auto mFenHeight = 720.0f;
static std::string MessageOPENCours("Analyse du document en cours");
static std::string MessagePercentOPENCours("12 %");
static wstring CheminPDG(L"");
static wstring CheminPDGuser(L"");
static wstring CheminBASE(L"");
static wstring CheminFontTampon(L"");
static string sCheminMONORoboto("");
static string sCheminMONORobotoBold("");
static string sCheminMONORobotoBoldItalic("");
static string sCheminMONORobotoItalic("");
static wstring CheminCompacteRepare(L"");
static wstring CheminPopplerPDFPPM(L"");
static wstring CheminPopplerPDFPPMTempOut(L"");
static wstring CheminPopplerPDFPPMTempOutMini(L"");
static wstring FichierPDFsortie(L"");
static wstring CheminTemp(L"");
static bool AfficheFenetreSpinner = false;
static bool AfficheFenetreSpinnerthreadFolioSansPDG = false;
static bool BoutonContinueCheminProcedure = false;
static bool BoutonContinueReferenceProcedure = true;
static bool AfficheReferenceProcedure = false;
static bool BoutonContinueTrancheProcedure = true;
static bool AfficheTrancheProcedure = false;
static bool BoutonContinueFolioProcedure = true;
static bool AfficheAnnuleFolioProcedure = false;
static bool BoutonAfficheAnnuleFolioProcedure = true;
static bool AfficheFolioProcedure = false;
static bool SetEnterTextKeyboardSavePDGtodisk = false;

static bool AfficheModalAvancementFoliotage = false;
static bool AfficheModalAvancementFoliotageSansPdg = false;
static bool AfficheModalGenerePageDeGarde = false;
static bool AfficheModalSupprimePageDeGarde = false;
static bool AfficheModalAnnulationDeFolio = false;
static bool AfficheModalNomPageDeGarde = false;

static bool bOpenPDF = false;
static std::vector<char> vecChar;
static bool done = false;
static bool SlowDown = false;

static std::string CheminPDForiginal("");
static std::wstring wCheminPDForiginal(L"");
static std::string CheminPDF("");
static std::wstring wCheminPDF(L"");
static std::string InformationPDF("");
static vector<PoDoFo::PdfRect> vecMediaBox;
static vector<int> vecRotation;
static vector<string> ListePDGModele;
static vector<string> ListePDGUser;
static string pdgClassSEED = "";
static int item_current_vPDG = 0;
static int item_current_vPDGuser = -1;
static char  txtSpinner[2] = "\0";
static char NomPDGtxt[128];
static char NomSite[32];
static char TrancheCode[10][6];
static char strREFERENCEREE[32] = "";
static char strINDICEREE[5] = "";
static char strINDICEREE_pdg[5] = "";
static char NomSite_pdg[32];
static bool TrancheSelect[10];
static bool ThemeSombre = false;
static bool tabFolioter = true;
static bool tabPageDeGarde = true;
static bool tabParametre = true;
static int radioTotalPartiel = 0;
static bool FolioProcedureAAnnuler[9999];
static bool FolioProcedureAAnnulerBKUP[9999];
static bool isGenereMiniature = false;
static bool isLoadingMiniature = false;
static bool DemarreFoliotageTASK = false;
static bool FoliotageGenerePDG = false;
static bool show_demo_window = false;
//GESTION TAB
static bool TabFolio_ouvert = true;
static bool TabPDG_ouvert = false;
static bool TabCouleur_ouvert = false;
static bool TabApropos_ouvert = false;
// Bascule Folio <> PDG
static bool AllerVersPDG = false;
static bool AllerVersFolio = false;
static bool FoliotageEnCours = false;
// Gestion miniature
static int NombreCoeur = 2;
static int GenereMiniaturefileCount = 0;
static int MiniatureSelectionnee = 0;
plf::nanotimer tmrGenereMiniature;
/*  0=HautGauche 1=HautDroite(DEFAUT) 2=BasGauche 3=BasDroite*/
static int radioEmplacementTampon = 1;
static ImU16 margeEmplacementTamponX = 20;
static ImU16 margeEmplacementTamponY = 20;
static const ImU16   u16_one = 1, u16_fast = 10;//Réglage du Scalar
static ImU16  u16_PageDebut = 1;
static ImU16  u16_PageFin = 1;
static ImU16  u16_PremierNumero = 3;
std::vector<TextureSTB> vecListeTexture;
std::vector<TextureSTB> vecListeTextureMaxiPathOnly;
static int CompteLoading = 0;
TextureSTB mApercuTexture;
vector<Couleur> ListeCouleur;
static float sListeCouleurTranche[10][4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static std::vector<std::wstring> LogErreur;

// (っ◔◡◔)っ 𝙋𝙤𝙞𝙣𝙩 𝙚𝙣𝙩𝙧𝙚𝙚 𝙥𝙧𝙤𝙜𝙧𝙖𝙢𝙢𝙚
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	NombreCoeur = (int)NombreCPU();
	MY_MSG("Init : %d cpu", NombreCoeur);

	TCHAR path[4096];
	DWORD length;
	length = GetModuleFileName(NULL, path, 4096);
	/*	Chemin de base de l'executable, fini par \		*/
	CheminBASE = path;
	CheminBASE = CheminBASE.substr(0, CheminBASE.find_last_of(L"\\") + 1);
	CheminPDG = CheminBASE + L"PDG_modele\\";
	CheminPDGuser = CheminBASE + L"PDG_utilisateur\\";
	CheminFontTampon = CheminBASE + L"Police\\Roboto-Regular.ttf";
	sCheminMONORoboto = fileHELPER.ConvertWideToANSI(CheminBASE) + "Police\\Roboto-Regular.ttf";
	sCheminMONORobotoBold = fileHELPER.ConvertWideToANSI(CheminBASE) + "Police\\Roboto-Bold.ttf";
	sCheminMONORobotoBoldItalic = fileHELPER.ConvertWideToANSI(CheminBASE) + "Police\\Roboto-BoldItalic.ttf";
	sCheminMONORobotoItalic = fileHELPER.ConvertWideToANSI(CheminBASE) + "Police\\Roboto-Italic.ttf";
	CheminCompacteRepare = CheminBASE + L"CompRepare\\CompacteRepareCommandLine.exe";
	CheminPopplerPDFPPM = CheminBASE + L"PdfToPPM\\pdftoppm.exe";
	CheminTemp = filesystem::temp_directory_path().wstring() + L"REEMAKER.TMP\\SESSION." + wGenerate(3);
	try
	{
		filesystem::create_directories(CheminTemp);
	}
	catch (const std::exception& e)
	{
		MY_TRACE("Exception a la ligne:  %s", e.what());
	}
	ListePDGModele.clear();
	pdgClassSEED = sGenerate(12);
	item_current_vPDG = 0;
	item_current_vPDGuser = -1;
	for (const auto& p : filesystem::directory_iterator(CheminPDG))
	{
		string pPathExtension = p.path().extension().string();
		transform(pPathExtension.begin(), pPathExtension.end(), pPathExtension.begin(), ::tolower);
		string pPathFileName = p.path().filename().string();
		transform(pPathFileName.begin(), pPathFileName.end(), pPathFileName.begin(), ::tolower);
		if (pPathExtension == ".txt")
			if (pPathFileName == "page de garde standard.txt")
				ListePDGModele.insert(ListePDGModele.begin() + 0, p.path().filename().u8string());
			else
				ListePDGModele.push_back(p.path().filename().u8string());
	}
	for (const auto& p : filesystem::directory_iterator(CheminPDGuser))
	{
		string pPathExtension = p.path().extension().string();
		transform(pPathExtension.begin(), pPathExtension.end(), pPathExtension.begin(), ::tolower);
		string pPathFileName = p.path().filename().string();
		transform(pPathFileName.begin(), pPathFileName.end(), pPathFileName.begin(), ::tolower);
		if (pPathExtension == ".txt")
			ListePDGUser.push_back(p.path().filename().u8string());
	}
	if (item_current_vPDGuser == -1 && ListePDGUser.size() > 0)
		item_current_vPDGuser = 0;

	if (ListePDGModele.size() != 0)
	{ //On charge la première page de garde
		mPDGHelper.SetBaseModelePath(CheminPDG);
		mPDGHelper.OpenAndParseConfig_v2(fileHELPER.ConvertUtf8ToWide(ListePDGModele[0]));
		TRACE_PDG("Nombre Objet à dessiner : %d\n", mPDGHelper.ItemCount());
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		#pragma error "Impossible d'initialiser la carte graphique"
		return -1;
	}
	// Setup window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#pragma warning( push )
#pragma warning( disable : 26812 )
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
#pragma warning( pop )
	SDL_Window* window = SDL_CreateWindow("REEMaker - 5.0 Initium Novum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	SDL_SetWindowMinimumSize(window, 1124, 720);
	{	/*	On récupère le HandleWINdows de la fenêtre*/
		struct SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window, &wmInfo);
		mHwnd = wmInfo.info.win.window;
	}
	/*  POLICE  */
	//Fusion de FONT AWESOME avec la police d'affichage principale
	ImFont* MYFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(sCheminMONORoboto.c_str(), 14.0f);
	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesome4_compressed_data, FontAwesome4_compressed_size, 14.0f, &config, icons_ranges);// Merge into first font
	io.Fonts->Build();
	//Police aditionnelle
	ImFont* MYFont20 = ImGui::GetIO().Fonts->AddFontFromFileTTF(sCheminMONORoboto.c_str(), 20.0f);
	ImFont* MYFont14bold = ImGui::GetIO().Fonts->AddFontFromFileTTF(sCheminMONORobotoBold.c_str(), 14.0f);
	ImFont* MYFont10bold = ImGui::GetIO().Fonts->AddFontFromFileTTF(sCheminMONORobotoBold.c_str(), 10.0f);
	/*  FIN POLICE  */
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();
	ListeCouleur.clear();
	Couleur mCouleur;
	{
		mCouleur.NomCouleur = ICO_TEXT_CSTR(ICON_FA_LOCK, " Rouge");
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 1.0f,0.0f,0.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
		mCouleur.NomCouleur = ICO_TEXT_CSTR(ICON_FA_LOCK, " Vert");
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 0.0f,1.0f,0.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
		mCouleur.NomCouleur = ICO_TEXT_CSTR(ICON_FA_LOCK, " Bleu");
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 0.0f,0.0f,1.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
		mCouleur.NomCouleur = ICO_TEXT_CSTR(ICON_FA_LOCK, " Noir");
		mCouleur.Locked = true;
		mCouleur.CodeCouleur = { 0.0f,0.0f,0.0f,1.0f };
		ListeCouleur.push_back(mCouleur);
	}
	/*	Charge les paramètres  */
	ChargeParametres();

	vecChar.resize(4);
	/*
	* Main loop
	*/
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT) {
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					if (event.window.windowID == SDL_GetWindowID(window))
					{
						MY_MSG(u8"MSG: Fenêtre %d fermée", event.window.windowID);
						done = true;
					}
					break;
				case SDL_WINDOWEVENT_SHOWN:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: Fenêtre %d affichée", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_HIDDEN:
				{
					SlowDown = true;
					MY_MSG(u8"MSG: Fenêtre %d cachée", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_EXPOSED:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: Fenêtre %d exposée", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_MOVED:
				{
					MY_MSG(u8"MSG: Fenêtre %d déplacée en %d,%d",
						event.window.windowID, event.window.data1,
						event.window.data2);
				}
				break;
				case SDL_WINDOWEVENT_RESIZED:
				{
					MY_MSG(u8"MSG: Fenêtre %d redimensionnée en %dx%d",
						event.window.windowID, event.window.data1,
						event.window.data2);
				}
				break;
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					MY_MSG(u8"MSG: Fenêtre %d dimension changée en %dx%d",
						event.window.windowID, event.window.data1,
						event.window.data2);
				}
				break;
				case SDL_WINDOWEVENT_MINIMIZED:
				{
					SlowDown = true;
					MY_MSG(u8"MSG: Fenêtre %d réduite", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_MAXIMIZED:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: Fenêtre %d maximisée", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_RESTORED:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: Fenêtre %d restaurée", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_ENTER:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: La souris entrée dans la fenêtre %d",
						event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_LEAVE:
				{
					SlowDown = true;
					MY_MSG(u8"MSG: La souris a quittée la fenêtre %d", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: La fenêtre %d a récupérée le focus clavier",
						event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					SlowDown = true;
					MY_MSG(u8"MSG: La fenêtre %d a perdue le focus clavier",
						event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_TAKE_FOCUS:
				{
					SlowDown = false;
					MY_MSG(u8"MSG: Fenêtre %d se voit à offrir le focus", event.window.windowID);
				}
				break;
				case SDL_WINDOWEVENT_HIT_TEST:
				{
					MY_MSG(u8"MSG: Fenêtre %d à reçu un test d'entrée", event.window.windowID);
				}
				break;
				default:
				{
					MY_MSG("MSG: Fenêtre %d evènement inconnue %d",
						event.window.windowID, event.window.event);
				}
				break;
				}
			}
		}
		if (ThemeSombre)
			ImGui::StyleColorsDarkCharcoal();
		else
			ImGui::StyleColorsLight();
		if (SlowDown)
			std::this_thread::sleep_for(120ms);
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
		static int width;
		static int height;
		{
			sprintf_s(txtSpinner, 2, "%c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
			txtSpinner[1] = '\0';
			/*
				╔═╗╔═╗╔╗╔╔═╗╔╦╗╦═╗╔═╗  ╔═╗╦═╗╦╔╗╔╔═╗╦╔═╗╔═╗╦  ╔═╗
				╠╣ ║╣ ║║║║╣  ║ ╠╦╝║╣   ╠═╝╠╦╝║║║║║  ║╠═╝╠═╣║  ║╣
				╚  ╚═╝╝╚╝╚═╝ ╩ ╩╚═╚═╝  ╩  ╩╚═╩╝╚╝╚═╝╩╩  ╩ ╩╩═╝╚═╝
			*/
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			static float HauteurBarrePrincipale = 30.0f;
			static float HauteurBarreSeconde = 34.0f;
			SDL_GetWindowSize(window, &width, &height);
			ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
			ImGui::Begin(u8"Fenètre principale", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);// Create a window called "Hello, world!" and append into it.
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0.0, 0.0), ImVec2(width, HauteurBarrePrincipale - 1.0f),
				ImGui::ColorConvertFloat4ToU32(ThemeSombre ? IMVEC4_COL16(255, 255, 255, 30) : IMVEC4_COL16(200, 200, 200, 200)));
#pragma region BoutonTAB
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0);
			if (TabFolio_ouvert)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FILE_PDF_O, u8" Folioter un REE")))
				{
					TabFolio_ouvert = true;
					TabPDG_ouvert = false;
					TabCouleur_ouvert = false;
					TabApropos_ouvert = false;
				}
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Tab));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FILE_PDF_O, u8" Folioter un REE")))
				{
					TabFolio_ouvert = true;
					TabPDG_ouvert = false;
					TabCouleur_ouvert = false;
					TabApropos_ouvert = false;
				}
				ImGui::PopStyleColor();
			}
			ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.0f);

			if (TabPDG_ouvert)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FILE_TEXT_O, u8" Page de garde")))
				{
					TabFolio_ouvert = false;
					TabPDG_ouvert = true;
					TabCouleur_ouvert = false;
					TabApropos_ouvert = false;
				}
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Tab));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FILE_TEXT_O, u8" Page de garde")))
				{
					TabFolio_ouvert = false;
					TabPDG_ouvert = true;
					TabCouleur_ouvert = false;
					TabApropos_ouvert = false;
				}
				ImGui::PopStyleColor();
			}
			ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.0f);

			if (TabCouleur_ouvert)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_COGS, u8" Options")))
				{
					TabFolio_ouvert = false;
					TabPDG_ouvert = false;
					TabCouleur_ouvert = true;
					TabApropos_ouvert = false;
				}
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Tab));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_COGS, u8" Options")))
				{
					TabFolio_ouvert = false;
					TabPDG_ouvert = false;
					TabCouleur_ouvert = true;
					TabApropos_ouvert = false;
				}
				ImGui::PopStyleColor();
			}
			ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.0f);

			if (TabApropos_ouvert)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_INFO, u8" A propos")))
				{
					TabFolio_ouvert = false;
					TabPDG_ouvert = false;
					TabCouleur_ouvert = false;
					TabApropos_ouvert = true;
				}
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Tab));
				if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_INFO, u8" A propos")))
				{
					TabFolio_ouvert = false;
					TabPDG_ouvert = false;
					TabCouleur_ouvert = false;
					TabApropos_ouvert = true;
				}
				ImGui::PopStyleColor();
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
#pragma endregion
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5.0);
			ImGui::Separator();

			if (AllerVersFolio)
			{
				AllerVersFolio = false; TabFolio_ouvert = true; TabPDG_ouvert = false; TabCouleur_ouvert = false; TabApropos_ouvert = false;
			}
			if (AllerVersPDG)
			{
				AllerVersPDG = false; TabFolio_ouvert = false; TabPDG_ouvert = true; TabCouleur_ouvert = false; TabApropos_ouvert = false;
			}

			if (TabFolio_ouvert)
			{
				if (ImGui::BeginTable("##TableTABFOLIO", 1, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, -1.0f)))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Chemin de la procédure :");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(u8"Chemin de la procédure :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Cliquer sur le bouton 'Rechercher' pour sélectionner la procédure PDF à passer REE."); /*ImGui::Text("");*/

					static char strCHEMINPROCEDURE[4096] = "";
					ImGui::SetNextItemWidth(ImGui::GetContentRegionMax().x - 280.0f - 10.0f);
					ImGui::InputTextWithHint(u8"##TextCheminProcedure", u8"Cliquer sur 'Rechercher' pour sélectionner la procédure à folioter", strCHEMINPROCEDURE, IM_ARRAYSIZE(strCHEMINPROCEDURE), ImGuiInputTextFlags_ReadOnly);

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 280.0f);
					if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FOLDER_OPEN, u8" Rechercher##OuvreProcedure"), ImVec2(280.0f, 22.0f)))
					{
						AfficheFenetreSpinner = true;
						MessageOPENCours = fmt::format("Analyse du document PDF");
						MessagePercentOPENCours = "";
						AfficheModalAvancementFoliotage = true;
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
						hr = pDlg->Show(mHwnd);
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
									wCheminPDForiginal = wstring(pwsz);
									CheminPDForiginal = fileHELPER.ConvertWideToUtf8(wCheminPDForiginal);
									CoTaskMemFree(pwsz);
									//On a le fichier PDF, on s'assure qu'il n'y a aucun reliquat en %TEMP%\REEMAKER.TMP\ %session% \*.pdf
									try
									{
										uintmax_t resRemove = filesystem::remove_all(CheminTemp);
										filesystem::create_directories(CheminTemp);
									}
									catch (const std::exception& e)
									{
										MY_TRACE("Exception a la ligne:  %s", e.what());
									}
									/* THREAD APPROPRIATION PDF (PDF Compacte et repare + Vector MediaBox*/
									std::thread t([]()
										{
											MessageOPENCours = fmt::format("Appropriation du document PDF...");
											MessagePercentOPENCours = "";
											wCheminPDF = CheminTemp + L"\\pdftemp_" + wGenerate(12) + L".pdf";
											CheminPDF = fileHELPER.ConvertWideToUtf8(wCheminPDF);

											SHELLEXECUTEINFO ShExecInfo;
											ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
											ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
											ShExecInfo.hwnd = NULL;
											ShExecInfo.lpVerb = NULL;
											ShExecInfo.lpFile = CheminCompacteRepare.c_str();
											wstring mParameter = fileHELPER.ConvertUtf8ToWide(fmt::format("comprep \"{}\" \"{}\"", CheminPDForiginal, CheminPDF));
											ShExecInfo.lpParameters = mParameter.c_str();
											ShExecInfo.lpDirectory = CheminBASE.c_str();
											ShExecInfo.nShow = SW_HIDE;
											ShExecInfo.hInstApp = NULL;
											ShellExecuteEx(&ShExecInfo);
											WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
											DWORD processExitCode;
											GetExitCodeProcess(ShExecInfo.hProcess, &processExitCode);
											CloseHandle(ShExecInfo.hProcess);
											if (processExitCode != 4)
												try
											{
												filesystem::copy_file(wCheminPDForiginal, wCheminPDF);
											}
											catch (const std::exception& e)
											{
												MY_TRACE("Exception a la ligne:  %s", e.what());
											}
											MessageOPENCours = fmt::format("Analyse du document PDF...");
											MessagePercentOPENCours = "";
											try
											{
												vecMediaBox.clear();
												vecRotation.clear();
												PoDoFo::PdfMemDocument documentSource(wCheminPDForiginal.c_str());
												int nbPages = documentSource.GetPageCount();
												for (size_t i = 0; i < nbPages; i++)
												{
													PoDoFo::PdfPage* pPage = documentSource.GetPage(i);
													vecMediaBox.push_back(pPage->GetMediaBox());
													vecRotation.push_back(pPage->GetRotation());
													MessageOPENCours = fmt::format(string("Analyse du document PDF ..." + string(txtSpinner)));
													MessagePercentOPENCours = fmt::format("{}%", (int)(((float)i / nbPages) * 100.0f) + 1);
												}
												string PdfVErsionToString = "";
												switch (documentSource.GetPdfVersion())
												{
												case PoDoFo::ePdfVersion_1_0:
													PdfVErsionToString = "PDF Version 1.0";
													break;
												case PoDoFo::ePdfVersion_1_1:
													PdfVErsionToString = "PDF Version 1.1";
													break;
												case PoDoFo::ePdfVersion_1_2:
													PdfVErsionToString = "PDF Version 1.2";
													break;
												case PoDoFo::ePdfVersion_1_3:
													PdfVErsionToString = "PDF Version 1.3";
													break;
												case PoDoFo::ePdfVersion_1_4:
													PdfVErsionToString = "PDF Version 1.4";
													break;
												case PoDoFo::ePdfVersion_1_5:
													PdfVErsionToString = "PDF Version 1.5";
													break;
												case PoDoFo::ePdfVersion_1_6:
													PdfVErsionToString = "PDF Version 1.6";
													break;
												case PoDoFo::ePdfVersion_1_7:
													PdfVErsionToString = "PDF Version 1.7";
													break;
												default:
													PdfVErsionToString = "PDF Version Inconnu";
													break;
												}
												std::string pdfDETAIL = u8"<vide>";
												std::string sAuteur = u8"<vide>";
												std::string sCreePar = u8"<vide>";
												std::string sDateTime = u8"<vide>";
												try
												{
													sDateTime = fmt::format(u8"{:%Y/%m/%d %H:%M:%S}", fmt::localtime(documentSource.GetInfo()->GetCreationDate().GetTime()));
												}
												catch (const std::exception&)
												{
												}
												try
												{
													sAuteur = documentSource.GetInfo()->GetAuthor().GetStringUtf8();
													if (sAuteur.length() > 20)
														sAuteur = sAuteur.substr(0, 20);
													if (sAuteur == "") sAuteur = u8"<vide>";
												}
												catch (const std::exception&)
												{
												}
												try
												{
													sCreePar = documentSource.GetInfo()->GetCreator().GetStringUtf8();
													if (sCreePar.length() > 20)
														sCreePar = sCreePar.substr(0, 20);
													if (sCreePar == "") sCreePar = u8"<vide>";
												}
												catch (const std::exception&)
												{
												}
												pdfDETAIL = fmt::format(u8"Auteur : {}, Crée par : {}, Le {}", sAuteur, sCreePar, sDateTime);
												InformationPDF = fmt::format(u8" {} page(s) pour {} Mo [{}, {}]", nbPages, filesystem::file_size(wCheminPDF) / 1024 / 1024, PdfVErsionToString, pdfDETAIL);
												memcpy_s(strCHEMINPROCEDURE, CheminPDForiginal.size(), &CheminPDForiginal[0], CheminPDForiginal.size());
												strCHEMINPROCEDURE[CheminPDForiginal.size()] = '\0';
												this_thread::sleep_for(500ms);
												BoutonContinueCheminProcedure = true;
											}
											catch (const PoDoFo::PdfError& e)
											{
												MY_TRACE("Exception a la ligne : %s", e.what());
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
											u16_PageDebut = (ImU16)1;
											u16_PageFin = (ImU16)vecRotation.size();
											BoutonContinueReferenceProcedure = true;
											AfficheReferenceProcedure = false;
											BoutonContinueTrancheProcedure = true;
											AfficheTrancheProcedure = false;
											BoutonContinueFolioProcedure = true;
											BoutonAfficheAnnuleFolioProcedure = true;
											AfficheAnnuleFolioProcedure = false;
											AfficheFolioProcedure = false;
											AfficheFenetreSpinner = false;
											for (int zz = 0; zz < 9999; zz++)
											{
												FolioProcedureAAnnuler[zz] = false;
											}
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

					ImGui::Text(u8"Information sur la procédure sélectionnée : ");
					ImGui::SameLine();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(InformationPDF.c_str());
					ImGui::PopFont();

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - BoutonSuivant.x);
					if (ImGuiAl::Button(ICO_TEXT_CSTR(ICON_FA_FORWARD, u8" Etape suivante##BoutonContinueCheminProcedure"), BoutonContinueCheminProcedure, BoutonSuivant))
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
						ImGui::SameLine(); HelpMarker(u8"Vous devez saisir la référence de la procédure à passer REE.\nIl faut saisir le site, le palier, REE, chantier ou non, système élémentaire, numéro de la procédure"); /*ImGui::Text("");*/
						/****************/
						ImGui::Text(u8"Nom du site : ");
						ImGui::SameLine();
						if (AfficheFolioProcedure)
							ImGui::PushFont(MYFont14bold);
						ImGui::SetNextItemWidth(180.0f);
						ImGui::InputTextWithHint("##NomDuSite", "ex. Flamanville", NomSite, IM_ARRAYSIZE(NomSite), AfficheFolioProcedure ? ImGuiInputTextFlags_ReadOnly : 0);
						if (AfficheFolioProcedure)
							ImGui::PopFont();
						ImGui::SameLine();
						/****************/
						ImGui::Text(u8"Référence de la procédure : ");
						ImGui::SameLine();
						if (AfficheFolioProcedure)
							ImGui::PushFont(MYFont14bold);
						ImGui::SetNextItemWidth(180.0f);
						ImGui::InputTextWithHint("##ReferenceProcedure", "ex. YR REE LHR 104", strREFERENCEREE, IM_ARRAYSIZE(strREFERENCEREE), AfficheFolioProcedure ? ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsUppercase : ImGuiInputTextFlags_CharsUppercase);
						if (AfficheFolioProcedure)
							ImGui::PopFont();
						ImGui::SameLine();
						/****************/
						ImGui::Text(u8"  Indice de la procédure : ");
						ImGui::SameLine();
						if (AfficheFolioProcedure)
							ImGui::PushFont(MYFont14bold);
						ImGui::SetNextItemWidth(80.0f);
						ImGui::InputTextWithHint("##IndiceProcedure", "ex. A ou PREL", strINDICEREE, IM_ARRAYSIZE(strINDICEREE), AfficheFolioProcedure ? ImGuiInputTextFlags_ReadOnly : 0);
						if (AfficheFolioProcedure)
							ImGui::PopFont();
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						if (!AfficheFolioProcedure)
							if (string(strREFERENCEREE).size() > 0 && string(strINDICEREE).size() > 0 && string(NomSite).size() > 0)
								BoutonContinueReferenceProcedure = true;
							else
								BoutonContinueReferenceProcedure = false;
						ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - BoutonSuivant.x - 2.0f);
						if (ImGuiAl::Button(ICO_TEXT_CSTR(ICON_FA_FORWARD, " Etape suivante##BoutonContinueReferenceProcedure"), BoutonContinueReferenceProcedure, BoutonSuivant))
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
						ImGui::SameLine(); HelpMarker(u8"Vous devez choisir si la procédure est foliotée totalement ou partiellement.\nVous allez aussi définir le premier numéro apparaissant sur le premier folio tamponné.");

						int cpChoix = radioTotalPartiel;
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						ImGui::RadioButton("Foliotage total", AfficheAnnuleFolioProcedure ? &cpChoix : &radioTotalPartiel, 0); ImGui::SameLine();
						ImGui::PopStyleVar();
						ImGui::Text("     "); ImGui::SameLine();
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						ImGui::RadioButton("Foliotage partiel", AfficheAnnuleFolioProcedure ? &cpChoix : &radioTotalPartiel, 1);
						ImGui::PopStyleVar();
						if (radioTotalPartiel == 1)
						{
							ImGui::SameLine(); ImGui::Text("  "); ImGui::SameLine();
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
								if (u16_PageFin > (ImU16)vecRotation.size())
									u16_PageFin = (ImU16)vecRotation.size();
							}
						}

						ImGui::Text(u8"Le folio du premier tampon sera : "); ImGui::SameLine();
						ImGui::SetNextItemWidth(100.0f);
						if (ImGui::InputScalar("##ScalarPremierFolio", ImGuiDataType_U16, &u16_PremierNumero, &u16_one, &u16_fast, "%u", BoutonContinueFolioProcedure ? 0 : ImGuiInputTextFlags_ReadOnly))
						{
						}

						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - BoutonSuivant.x);
						if (ImGuiAl::Button(ICO_TEXT_CSTR(ICON_FA_FORWARD, u8" Etape suivante##BoutonContinueFolioProcedure"), BoutonContinueFolioProcedure, BoutonSuivant))
						{
							BoutonContinueFolioProcedure = false;
							AfficheAnnuleFolioProcedure = true;
						}
						ImGui::PopStyleVar();
					}

					if (AfficheAnnuleFolioProcedure)
					{
						ImGui::Separator();
						ImGui::PushFont(MYFont14bold);
						ImGui::Text(u8"Annuler un ou plusieurs folio(s) :");
						ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f),
							ImVec2(ImGui::CalcTextSize(u8"Annuler un ou plusieurs folio(s) :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
						ImGui::PopFont();
						ImGui::SameLine(); HelpMarker(u8"Vous devez choisir si vous souhaitez que un/des folio(s) soit annulés (trait qui barre la page).");
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						if (ImGui::Button(u8"Choisir les folio à annuler##BoutonAfficheFenetreAnnulation", ImVec2(300.0f, 22.0f)))
						{
							CheminPopplerPDFPPMTempOut = CheminTemp + L"\\" + wGenerate(8) + L"." + wGenerate(3);
							CheminPopplerPDFPPMTempOutMini = CheminTemp + L"\\" + wGenerate(8) + L"." + wGenerate(3);
							isGenereMiniature = true;
							isLoadingMiniature = true;
							MiniatureSelectionnee = 0;
							vecListeTexture.clear();
							vecListeTextureMaxiPathOnly.clear();
							CompteLoading = 0;
							tmrGenereMiniature.start();
							GenereMiniaturefileCount = 0;
							for (size_t zz = 0; zz < 9999; zz++)
							{
								FolioProcedureAAnnulerBKUP[zz] = FolioProcedureAAnnuler[zz];
							}
							AfficheModalAnnulationDeFolio = true;
							//THREAD
							std::thread tTHREAD([]() {
								{
									const int BlocPage = 20;
									const int nbTotalThread = NombreCoeur;
									ctpl::thread_pool p(nbTotalThread);

									int nbPageTotal = vecMediaBox.size();
									vector<int> vecDOCPDF;
									for (size_t i = 0; i < nbPageTotal; i++)
										vecDOCPDF.push_back(i + 1);
									try
									{
										filesystem::create_directories(CheminPopplerPDFPPMTempOut);
									}
									catch (const std::exception& e)
									{
										MY_TRACE("Exception a la ligne:  %s", e.what());
									}
									if (vecDOCPDF.size() <= BlocPage)
										std::future<bool> qw = p.push(GenereMiniature, 1, vecDOCPDF.size(), wCheminPDF);
									else
									{
										while (true)
										{
											if (vecDOCPDF.size() < BlocPage)
											{
												std::future<bool> qw = p.push(GenereMiniature, vecDOCPDF[0], vecDOCPDF[vecDOCPDF.size() - 1], wCheminPDF);
												break;
											}
											std::future<bool> qw = p.push(GenereMiniature, vecDOCPDF[0], vecDOCPDF[BlocPage - 1], wCheminPDF);
											vecDOCPDF.erase(vecDOCPDF.begin() + 0, vecDOCPDF.begin() + BlocPage);
										}
									}
									p.stop(true);
									auto dirIter = std::filesystem::directory_iterator(CheminPopplerPDFPPMTempOut);
									try {
										for (auto& entry : dirIter)
										{
											if (entry.is_regular_file())
											{
												TextureSTB mTex;
												mTex.Chemin = entry.path().string();
												mTex.my_image_success_loading = false;
												vecListeTextureMaxiPathOnly.push_back(mTex);
											}
										}
									}
									catch (const std::exception& e)
									{
										MY_TRACE("Exception a la ligne:  %s", e.what());
									}
									MY_MSG("Fin génération miniature en %sms", to_string(tmrGenereMiniature.get_elapsed_ms()).c_str());
								}
								{
									const int BlocPage = 20;
									const int nbTotalThread = NombreCoeur;
									ctpl::thread_pool p(nbTotalThread);

									int nbPageTotal = vecMediaBox.size();
									vector<int> vecDOCPDF;
									for (int i = 0; i < nbPageTotal; i++)
										vecDOCPDF.push_back(i + 1);
									try
									{
										filesystem::create_directories(CheminPopplerPDFPPMTempOutMini);
									}
									catch (const std::exception& e)
									{
										MY_TRACE("Exception a la ligne:  %s", e.what());
									}
									if (vecDOCPDF.size() <= BlocPage)
										std::future<bool> qw = p.push(GenereMiniatureMIN, 1, vecDOCPDF.size(), wCheminPDF);
									else
									{
										while (true)
										{
											if (vecDOCPDF.size() < BlocPage)
											{
												std::future<bool> qw = p.push(GenereMiniatureMIN, vecDOCPDF[0], vecDOCPDF[vecDOCPDF.size() - 1], wCheminPDF);
												break;
											}
											std::future<bool> qw = p.push(GenereMiniatureMIN, vecDOCPDF[0], vecDOCPDF[BlocPage - 1], wCheminPDF);
											vecDOCPDF.erase(vecDOCPDF.begin() + 0, vecDOCPDF.begin() + BlocPage);
										}
									}
									p.stop(true);
									auto dirIter = std::filesystem::directory_iterator(CheminPopplerPDFPPMTempOutMini);
									try {
										for (auto& entry : dirIter)
										{
											if (entry.is_regular_file())
											{
												TextureSTB mTex;
												mTex.Chemin = entry.path().string();
												mTex.my_image_success_loading = false;
												vecListeTexture.push_back(mTex);
											}
										}
									}
									catch (const std::exception& e)
									{
										MY_TRACE("Exception a la ligne:  %s", e.what());
									}

									isGenereMiniature = false;
									MY_MSG("Fin génération miniature 64px en %sms", to_string(tmrGenereMiniature.get_elapsed_ms()).c_str());
								}
								});
							tTHREAD.detach();
						}
						ImGui::SameLine();
						int nbCancel = 0;
						for (size_t i = 0; i < vecMediaBox.size(); i++)
							if (FolioProcedureAAnnuler[i] == true)
								nbCancel++;
						ImGui::Text(ICO_TEXT_CSTR(ICON_FA_TIMES, fmt::format(u8"  {} folio(s) annulé(s)", nbCancel)));
						ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - BoutonSuivant.x);
						if (ImGuiAl::Button(ICO_TEXT_CSTR(ICON_FA_FORWARD, u8" Etape suivante##BoutonAfficheAnnuleFolioProcedure"), BoutonAfficheAnnuleFolioProcedure, BoutonSuivant))
						{
							BoutonAfficheAnnuleFolioProcedure = false;
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
						ImGui::SameLine(); HelpMarker(u8"Vous allez cocher les tranches applicables au REE et saisir le code projet.\nLe code projet est sous la forme XYYZZ avec :\nX = Le type de Cycle\nYY = L'index du cycle\nZZ = L'année du cycle"); ImGui::Text("");
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
								if (ImGui::Button(u8"Folioter avec une page de garde", ImVec2(-1.0f, 40.0f)))
								{
									AllerVersPDG = true;
									FoliotageEnCours = true;
								}
								ImGui::PopStyleVar();

								ImGui::TableSetColumnIndex(1);
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
								if (ImGui::Button(u8"Folioter sans page de garde", ImVec2(-1.0f, 40.0f)))
								{
									DemarreFoliotageTASK = false;
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
										wstring CustomCheminPDF = wCheminPDForiginal;
										CustomCheminPDF = CustomCheminPDF.substr(0, CustomCheminPDF.length() - 4);
										CustomCheminPDF += L"_foliotée";
										pDlg->SetFileName(CustomCheminPDF.c_str());
										pDlg->SetDefaultExtension(L"pdf");
										hr = pDlg->Show(mHwnd);
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
													FoliotageGenerePDG = false;//Ce sera un foliotage sans page de garde
													DemarreFoliotageTASK = true;
													FichierPDFsortie = wstring(pwsz);
													MY_MSG("MSG: Enregistrer sous %s", string(FichierPDFsortie.begin(), FichierPDFsortie.end()));
													CoTaskMemFree(pwsz);
												}
											}
										}
										else
										{
											DemarreFoliotageTASK = false;
											FichierPDFsortie = L"";
										}
										break;
									}
								}
								ImGui::PopStyleVar();
								ImGui::EndTable();
							}
					}
					ImGui::EndTable();
				}
			}
			if (TabPDG_ouvert)
			{
				if (ImGui::BeginTable("##TableTABPDG", 1, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, -1.0f)))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Button("##vide", ImVec2(0.01f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(ICO_TEXT_CSTR(ICON_FA_DATABASE, u8" "));
					ImGui::SameLine();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Base de modèle");
					ImGui::PopFont();
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
					ImGui::Text(u8"Intégré :");
					ImGui::SameLine();
					const char* combo_labelPDGModele = ListePDGModele[item_current_vPDG].c_str();  // Label to preview before opening the combo (technically it could be anything)
					const char* combo_labelPDGUser = (item_current_vPDGuser == -1) ? "" : ListePDGUser[item_current_vPDGuser].c_str();  // Label to preview before opening the combo (technically it could be anything)
					ImGui::SetNextItemWidth(250.0f);
					if (ImGui::BeginCombo("##CBBoxModelePDG", combo_labelPDGModele, 0 /*flags*/))
					{
						for (int n = 0; n < ListePDGModele.size(); n++)
						{
							const bool is_selected = (bool)(item_current_vPDG == n);
							if (ImGui::Selectable(ListePDGModele[n].c_str(), is_selected))
							{
								item_current_vPDG = n;
								{
									//Ouverture du fichier pour lecture
									mPDGHelper.ClearList();
									mPDGHelper.SetBaseModelePath(CheminPDG);
									mPDGHelper.OpenAndParseConfig_v2(fileHELPER.ConvertUtf8ToWide(ListePDGModele[item_current_vPDG]));
								}
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
					ImGui::Text(u8"Utilisateur :");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(300.0f);
					if (ImGui::BeginCombo("##CBBoxPDGutilisateur", combo_labelPDGUser, 0 /*flags*/))
					{
						for (int n = 0; n < ListePDGUser.size(); n++)
						{
							const bool is_selected = (bool)(item_current_vPDGuser == n);
							if (ImGui::Selectable(ListePDGUser[n].c_str(), is_selected))
							{
								item_current_vPDGuser = n;
								{
									//Ouverture du fichier pour lecture
									mPDGHelper.ClearList();
									mPDGHelper.SetBaseModelePath(CheminPDGuser);
									mPDGHelper.OpenAndParseConfig_v2(fileHELPER.ConvertUtf8ToWide(ListePDGUser[item_current_vPDGuser]));
								}
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_TRASH, u8" Supprimer cette page de garde utilisateur")))
					{
						if (ListePDGUser.size() >= 0 && item_current_vPDGuser != -1)
							AfficheModalSupprimePageDeGarde = true;
					}
					ImGui::PopStyleVar();
					ImGui::Separator();
					ImGui::Button("##vide", ImVec2(0.01f, 0.0f));
					ImGui::SameLine();
					ImGui::Text(ICO_TEXT_CSTR(ICON_FA_FILE_O, std::string(u8" Page de garde en cours d'utilisation  <<").c_str()));
					ImGui::SameLine();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(mPDGHelper.DocumentOuvertUTF8().c_str());
					ImGui::PopFont();
					ImGui::SameLine();
					ImGui::Text(u8">>");
					ImGui::Separator();
					/*
					* DEBUT DESSIN DYNAMIQUE PDG
					*/
					if (ImGui::BeginTable("##tablePdgDynamique", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY, ImVec2(-1.0f, ImGui::GetContentRegionMax().y - 110.0f - 26.0f)))
					{
						ImGui::TableSetupColumn(u8"Obl.##tablePdgDynamiqueObligatoire", ImGuiTableColumnFlags_WidthFixed, 25.0f, 0);
						ImGui::TableSetupColumn(u8"Question##tablePdgDynamiqueQuestion", ImGuiTableColumnFlags_WidthFixed, 350.0f, 1);
						ImGui::TableSetupColumn(u8"Aide##tablePdgDynamiqueAide", ImGuiTableColumnFlags_WidthFixed, 25.0f, 2);
						ImGui::TableSetupColumn(u8" Champ à renseigner##tablePdgDynamiqueLibéllé", ImGuiTableColumnFlags_WidthStretch, 3);
						ImGui::TableHeadersRow();

						for (size_t lPDG = 0; lPDG < mPDGHelper.ListeQuestion.size(); lPDG++)
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							if (mPDGHelper.ListeQuestion[lPDG].Obligatoire)
								ObligatoireMarker(u8"Doit être obligatoirement renseigné"); ImGui::SameLine();

							ImGui::TableSetColumnIndex(1);
							if (mPDGHelper.ListeQuestion[lPDG].Obligatoire)
							{
								ImGui::PushFont(MYFont14bold);
								ImGui::Text(mPDGHelper.ListeQuestion[lPDG].LaQuestion.c_str());
								ImGui::PopFont();
							}
							else
								ImGui::Text(mPDGHelper.ListeQuestion[lPDG].LaQuestion.c_str());
							ImGui::TableSetColumnIndex(2);
							HelpMarker(mPDGHelper.ListeQuestion[lPDG].AideQuestion.c_str());
							ImGui::TableSetColumnIndex(3);
							if (mPDGHelper.ListeQuestion[lPDG].EstCheckbox)
							{
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
								ImGui::Checkbox(string("##CheckBox" + pdgClassSEED + to_string(lPDG)).c_str(), &mPDGHelper.ListeQuestion[lPDG].CheckboxValue);
								ImGui::PopStyleVar();
							} 
							else if (mPDGHelper.ListeQuestion[lPDG].EstLigneTexte)
							{
								ImGuiInputTextFlags cFlag = ImGuiInputTextFlags_None;
								if (mPDGHelper.ListeQuestion[lPDG].EstMajuscule)
									cFlag = cFlag | ImGuiInputTextFlags_CharsUppercase;
								if (mPDGHelper.ListeQuestion[lPDG].EstChiffre)
									cFlag = cFlag | ImGuiInputTextFlags_CharsDecimal;
								ImGui::SetNextItemWidth(-1.0f);
								ImGui::InputText(string("##TexteUneLigne" + pdgClassSEED + to_string(lPDG)).c_str(),
									&mPDGHelper.ListeQuestion[lPDG].vDefautQuestion[0], mPDGHelper.ListeQuestion[lPDG].vDefautQuestion.size(), cFlag);
							}
							else if (mPDGHelper.ListeQuestion[lPDG].EstMultiLigneTexte)
							{
								ImGuiInputTextFlags cFlag = ImGuiInputTextFlags_None;
								if (mPDGHelper.ListeQuestion[lPDG].EstMajuscule)
									cFlag = cFlag | ImGuiInputTextFlags_CharsUppercase;
								if (mPDGHelper.ListeQuestion[lPDG].EstChiffre)
									cFlag = cFlag | ImGuiInputTextFlags_CharsDecimal;
								ImGui::InputTextMultiline(string("##TexteMultiLigne" + pdgClassSEED + to_string(lPDG)).c_str(),
									&mPDGHelper.ListeQuestion[lPDG].vDefautQuestion[0], mPDGHelper.ListeQuestion[lPDG].vDefautQuestion.size(), ImVec2(-1.0f, 40.0f), cFlag);
							}
							else
							{
								OutputDebugString(L"");
							}
						}
						ImGui::EndTable();
					}
					if (ImGui::BeginTable("##TableTABPDGbas", 1, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, 34.0f)))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FLOPPY_O, u8" Sauvegarder dans les pages de gardes utilisateurs")))
						{
							for (size_t i = 0; i < 128; i++)
								NomPDGtxt[i] = '\0';
							std::string Defautstr = u8"Dossier_REE_Numéro_Indice";
							memcpy_s(NomPDGtxt, Defautstr.size(), &Defautstr[0], Defautstr.size());

							AfficheModalNomPageDeGarde = true;
							SetEnterTextKeyboardSavePDGtodisk = true;
						}
						ImGui::PopStyleVar();
						ImGui::SameLine();
						if (FoliotageEnCours)
						{
							ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 225.0f);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_CHECK_SQUARE, u8" Démarrer le foliotage")))
							{
								DemarreFoliotageTASK = false;
								//Sauvegarder sous

								HRESULT hr;
								CComPtr<IFileSaveDialog> pDlg;
								COMDLG_FILTERSPEC aFileTypes[] = { { L"Fichier PDF", L"*.pdf" } };
								hr = pDlg.CoCreateInstance(__uuidof(FileSaveDialog));
								if (FAILED(hr))
									break;
								pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
								pDlg->SetTitle(L"Enregistrer la procédure foliotée avec page de garde");
								pDlg->SetOkButtonLabel(L"&Sauvegarder");
								wstring CustomCheminPDF = wCheminPDForiginal;
								CustomCheminPDF = CustomCheminPDF.substr(0, CustomCheminPDF.length() - 4);
								CustomCheminPDF += L"_foliotée";
								pDlg->SetFileName(CustomCheminPDF.c_str());
								pDlg->SetDefaultExtension(L"pdf");
								hr = pDlg->Show(mHwnd);
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
											DemarreFoliotageTASK = true;
											FoliotageGenerePDG = true;//Ce sera un foliotage sans page de garde
											AllerVersFolio = true;
											FoliotageEnCours = false;
											FichierPDFsortie = wstring(pwsz);
											MY_MSG("MSG: Enregistrer sous %s", string(FichierPDFsortie.begin(), FichierPDFsortie.end()));
											CoTaskMemFree(pwsz);
										}
									}
								}
								else
								{
									DemarreFoliotageTASK = false;
									FichierPDFsortie = L"";
								}
							}
							ImGui::SameLine();
							if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_UNDO, u8" Annuler")))
							{
								AllerVersFolio = true;
								FoliotageEnCours = false;
							}
							ImGui::PopStyleVar();
						}
						else
						{
							ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 220.0f);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							if (ImGui::Button(ICO_TEXT_CSTR(ICON_FA_FILE_PDF_O, u8" Générer cette page de garde en PDF")))
							{
								if (mPDGHelper.DocumentOuvertWIDE() != L"")
									AfficheModalGenerePageDeGarde = true;
							}
							ImGui::PopStyleVar();
						}
						ImGui::EndTable();
					}
					ImGui::EndTable();
				}
			}
			if (TabCouleur_ouvert)
			{
				if (ImGui::BeginTable("##TableCouleur", 1, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, -1.0f)))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Couleurs prédéfinies :");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(u8"Couleurs prédéfinies :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Vous pouvez retrouver la liste des couleurs disponibles pour les tampons / pages de gardes."); ImGui::Text("");

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
						ImGui::SameLine(); HelpMarker(u8"Vous allez pouvoir ajouter une nouvelle couleur à la liste des couleurs prédéfinies."); ImGui::Text("");

						ImGui::EndTable();
					}
					if (ImGui::BeginTable("##TableCouleurGauche", 1, ImGuiTableFlags_ScrollY, ImVec2(400.0f, 120.0f)))
					{
						static int PressedSelectable = -1;
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						for (size_t i = 0; i < ListeCouleur.size(); i++)
						{
							if (ImGui::Selectable(string(ListeCouleur[i].NomCouleur + "##sCB" + to_string(i) + ListeCouleur[i].NomCouleur).c_str(), false/*&ListeCouleurSelectable[i]*/, ImGuiSelectableFlags_None, ImVec2(200.0f, 20.0f)))
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
									MY_MSG("Suppression de %d", PressedSelectable);
								}
							if (ImGui::Selectable(u8"Annuler"))
							{
								MY_MSG("Annuler suppression de couleur %d", PressedSelectable);
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
						ImGui::Text(ICO_TEXT_CSTR(ICON_FA_PAINT_BRUSH, u8" Couleur du tampon tranche %d :"), k);
						ImGui::SameLine();
						ImGui::ColorEdit4(string("##CouleurTranche" + to_string(k)).c_str(), sListeCouleurTranche[k], ImGuiColorEditFlags_NoAlpha);
					}
					ImGui::Separator();
					ImGui::PushFont(MYFont14bold);
					ImGui::Text(u8"Autres paramètres :");
					ImGui::GetWindowDrawList()->AddLine(ImVec2(0.0f + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImVec2(ImGui::CalcTextSize(
						u8" Autres paramètres :").x + 8.0f, ImGui::GetCursorScreenPos().y - 4.0f), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
					ImGui::PopFont();
					ImGui::SameLine(); HelpMarker(u8"Choix de l'emplacement sur la page ou le tampon sera apposé, marge, ouverture des documents PDF après leurs générations..."); ImGui::Text("");

					if (ImGui::BeginTable("##TableAutreParamètre", 2, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, -1.0)))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(ICO_TEXT_CSTR(ICON_FA_ALIGN_LEFT, u8" Emplacement du tampon :")); /*ImGui::SameLine();*/
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
						ImGui::Dummy(ImVec2(20.0f, 26.0f)); ImGui::SameLine();
						ImGui::RadioButton(u8"En haut à gauche", &radioEmplacementTampon, 0); ImGui::SameLine();
						ImGui::RadioButton(u8"En haut à droite", &radioEmplacementTampon, 1); ImGui::SameLine();
						ImGui::RadioButton(u8"En bas à gauche", &radioEmplacementTampon, 2); ImGui::SameLine();
						ImGui::RadioButton(u8"En bas à droite", &radioEmplacementTampon, 3);
						ImGui::PopStyleVar();
						ImGui::Text(ICO_TEXT_CSTR(ICON_FA_INDENT, u8" Marge du tampon :")); /*ImGui::SameLine(); */
						ImGui::Dummy(ImVec2(20.0f, 26.0f)); ImGui::SameLine();
						ImGui::Text(ICO_TEXT_CSTR(ICON_FA_ARROWS_H, u8" Largeur en point : ")); ImGui::SameLine();
						ImGui::SetNextItemWidth(110.0f);
						ImGui::InputScalar("##ScalarMargeX", ImGuiDataType_U16, &margeEmplacementTamponX, &u16_one, &u16_fast, "%u");
						ImGui::SameLine(); ImGui::Text(ICO_TEXT_CSTR(ICON_FA_ARROWS_V, u8" Hauteur en point : ")); ImGui::SameLine();
						ImGui::SetNextItemWidth(110.0f);
						ImGui::InputScalar("##ScalarMargeY", ImGuiDataType_U16, &margeEmplacementTamponY, &u16_one, &u16_fast, "%u");

						ImGui::TableSetColumnIndex(1);
						ImGui::Text(ICO_TEXT_CSTR(ICON_FA_FILE_PDF_O, u8" Ouvrir les documents PDF après leurs génération : ")); ImGui::SameLine();
						ImGui::Checkbox("##cbOpenPDF", &bOpenPDF);

						ImGui::EndTable();
					}
					ImGui::EndTable();
				}
			}
			if (TabApropos_ouvert)
			{
				if (ImGui::BeginTable("##TableAPROPOS", 1, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, -1.0f)))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					const std::string markdownText = u8R"(
### REEMaker
Version 5.0 (Initium Novum)
Développé en C++ 17 sous Visual Studio 2019
Par Grégory WENTZEL [gregory.wentzel@edf.fr](mailto:gregory.wentzel@edf.fr)
Programme sous licence GPL 3

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
  * [PoDoFo Version 0.9.7](http://podofo.sourceforge.net)
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
  * [FMT](https://github.com/fmtlib/fmt/)
	Librairie de formatage de texte.
	[Licence 'MIT'](https://raw.githubusercontent.com/fmtlib/fmt/master/LICENSE.rst).

## Les executables suivants sont utilisés :
  * [Poppler PDFToPPM](https://poppler.freedesktop.org)
	Transformation de PDF en image.
	[Licence 'GPL3'](https://gitlab.freedesktop.org/poppler/poppler#history-and-gpl-licensing).
  * Compacte et Répare (Outil maison)
	Compresse et répare des fichiers PDF corrompus ou trop volumineux.
	[Licence 'AGPL'](https://itextpdf.com/en/how-buy/agpl-license).

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
  * [Roboto et Roboto Mono par Steve Matteson](https://github.com/google/fonts/tree/main/apache)
	[Licence 'Apache License'](https://github.com/google/fonts/blob/main/apache/roboto/LICENSE.txt).

)";
					static ImGui::MarkdownConfig mdConfig;
					mdConfig.linkCallback = [](ImGui::MarkdownLinkCallbackData data_)
					{
						std::string url(data_.link, data_.linkLength);
						if (!data_.isImage)
							ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
					};
					mdConfig.linkIcon = ICON_FA_LINK;
					mdConfig.headingFormats[0] = { MYFont14bold, false };//H1 #
					mdConfig.headingFormats[1] = { MYFont14bold, true }; //H2 ##
					mdConfig.headingFormats[2] = { MYFont20, false };    //H3 ###
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
					ImGui::EndTable();
				}
			}

			/*
			* Affichage Mode sombre / Clair
			* + FPS
			*/
			ImGui::SetCursorPosX(width - ImGui::CalcTextSize("(60.0 FPS)").x - ImGui::CalcTextSize("Thème sombre  ").x - /*Bordure*/24.0f - /*Checkbox*/24.0f);
			ImGui::SetCursorPosY(4.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
			ImGui::PushFont(MYFont10bold);
			ImGui::Checkbox("##ThemeSombre", &ThemeSombre);// Height = (FontSize + style.FramePadding.y * 2)
			ImGui::PopFont();
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::Text(u8"Thème sombre");
#ifdef DEBUG
			ImGui::SameLine();
			ImGui::PushFont(MYFont10bold);
			ImGui::SetCursorPosY(0.0f);
			ImGui::SetCursorPosX(width - ImGui::CalcTextSize("OOOOOO").x - 10.0f);
			ImGui::Text(SlowDown ? "Inactif" : "Actif");
			ImGui::SetCursorPosX(width - ImGui::CalcTextSize("OOOOOO").x - 10.0f);
			ImGui::SetCursorPosY(16.0f);
			ImGui::Text("%.1f Ips", ImGui::GetIO().Framerate);
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::SetCursorPosX(700.0f);
			//ImGui::PushFont(MYFont10bold);
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			//ImGui::PopFont();
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
#endif // DEBUG
			ImGui::End();
		}


		//Fonction déclenché par BOOL
		if (DemarreFoliotageTASK)
		{
			DemarreFoliotageTASK = false;
			AfficheFenetreSpinnerthreadFolioSansPDG = true;
			MessageOPENCours = fmt::format(u8"Création des tampons {}", string(txtSpinner));
			MessagePercentOPENCours = fmt::format("");
			AfficheModalAvancementFoliotageSansPdg = true;

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
					std::vector<std::wstring> mLogErreur;
					mLogErreur.clear();
					mLogErreur.push_back(L"Les documents suivants n'ont pu être créés :");

					MessageOPENCours = fmt::format(u8"Création des tampons {}", string(txtSpinner));
					MessagePercentOPENCours = fmt::format("");

					for (size_t t = 0; t < 10; t++)
					{
						if (TrancheSelect[t] == false)
							continue;

						try
						{
							wstring TR_FichierPDFSortie = FichierPDFsortie.substr(0, FichierPDFsortie.size() - 4) + L"_TR" + to_wstring(t) + L".pdf";
							{
								size_t mStarting = 0;
								size_t mEnding = vecMediaBox.size();
								if (radioTotalPartiel == 1)//Partiel
								{
									mStarting = u16_PageDebut - 1;
									mEnding = u16_PageFin;
								}
								bool PeutEtreCree = fileHELPER.JeSuisEcrivable(TR_FichierPDFSortie);
								if (PeutEtreCree)
								{
									PoDoFo::PdfMemDocument document(wCheminPDF.c_str());
									PoDoFo::PdfFont* pFont = document.CreateFontSubset("Roboto", true, false, false, PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), fileHELPER.ConvertWideToANSI(CheminFontTampon).c_str());
									pFont->SetFontSize(TamponPolice);
									for (size_t i = mStarting; i < mEnding; i++)
									{
										MessageOPENCours = fmt::format(u8"Création des tampons {}\n\nPage {} / {}", string(txtSpinner), i + 1, document.GetPageCount());
										MessagePercentOPENCours = fmt::format("Tr.{}", t);
										PoDoFo::PdfPage* pPage = document.GetPage(i);
										{
											if (FolioProcedureAAnnuler[i])
											{
												const double DualSpace = 120.0;
												const double SingleSpace = 60.0;

												PoDoFo::PdfPainter painter;
												PoDoFo::PdfRect rect(0, 0, vecMediaBox[i].GetWidth() - DualSpace, vecMediaBox[i].GetHeight() - DualSpace);

												PoDoFo::PdfXObject xObj(rect, &document);
												painter.SetPage(&xObj);
												painter.SetStrokeWidth(2.0);
												painter.SetColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);
												painter.SetFont(pFont);
												pFont->SetFontSize(70.0);
												PoDoFo::PdfString TexteFolioAnnulee = PoDoFo::PdfString("Sans Objet");
												if (vecRotation[i] == 0)
												{
													painter.SetStrokingColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);
													painter.DrawLine(0.0, 0.0, 0.0, vecMediaBox[i].GetHeight() - DualSpace);
													painter.Stroke();
													painter.DrawLine(0.0, vecMediaBox[i].GetHeight() - DualSpace,
														vecMediaBox[i].GetWidth() - DualSpace, 0.0);
													painter.Stroke();
													painter.DrawMultiLineText(0.0, 0.0, rect.GetWidth(), rect.GetHeight(),
														TexteFolioAnnulee,
														PoDoFo::EPdfAlignment::ePdfAlignment_Center, PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center);//OK
												}
												else if (vecRotation[i] == 90)
												{
													painter.SetTransformationMatrix(0.0, 1.0, -1.0, 0.0, (double)vecMediaBox[i].GetHeight() - DualSpace, 0.0);
													painter.SetStrokingColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);
													painter.DrawLine(0.0, -(vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight()),
														0.0, vecMediaBox[i].GetHeight() - DualSpace);
													painter.Stroke();
													painter.DrawLine(vecMediaBox[i].GetHeight() - DualSpace, -(vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight()),
														0.0, vecMediaBox[i].GetHeight() - DualSpace);
													painter.Stroke();
													painter.DrawMultiLineText(0.0, -(vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight()), rect.GetHeight(), rect.GetWidth(),
														TexteFolioAnnulee,
														PoDoFo::EPdfAlignment::ePdfAlignment_Center, PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center);//OK
												}
												else if (vecRotation[i] == 180)
												{
													painter.SetTransformationMatrix(-1.0, 0.0, 0.0, -1.0, (double)vecMediaBox[i].GetWidth() - DualSpace, (double)vecMediaBox[i].GetHeight() - DualSpace);
													painter.SetStrokingColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);
													painter.DrawLine(0.0, 0.0, 0.0, vecMediaBox[i].GetHeight() - DualSpace);
													painter.Stroke();
													painter.DrawLine(0.0, vecMediaBox[i].GetHeight() - DualSpace,
														vecMediaBox[i].GetWidth() - DualSpace, 0.0);
													painter.Stroke();
													painter.DrawMultiLineText(0.0, 0.0, rect.GetWidth(), rect.GetHeight(),
														TexteFolioAnnulee,
														PoDoFo::EPdfAlignment::ePdfAlignment_Center, PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center);//OK
												}
												else if (vecRotation[i] == 270)
												{
													painter.SetTransformationMatrix(0.0, -1.0, 1.0, 0.0, 0.0, (double)vecMediaBox[i].GetWidth() - DualSpace);
													painter.SetStrokingColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);
													painter.DrawLine(0.0 + vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight(), 0.0,
														0.0 + vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight(), vecMediaBox[i].GetWidth() - DualSpace);
													painter.Stroke();
													painter.DrawLine(0.0 + vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight(), vecMediaBox[i].GetWidth() - DualSpace,
														vecMediaBox[i].GetWidth() - DualSpace, 0.0);
													painter.Stroke();
													painter.DrawMultiLineText(0.0 + vecMediaBox[i].GetWidth() - vecMediaBox[i].GetHeight(), 0.0,
														vecMediaBox[i].GetHeight() - DualSpace, vecMediaBox[i].GetWidth() - DualSpace,
														TexteFolioAnnulee,
														PoDoFo::EPdfAlignment::ePdfAlignment_Center, PoDoFo::EPdfVerticalAlignment::ePdfVerticalAlignment_Center);//OK
												}
												rect.SetLeft(SingleSpace);
												rect.SetBottom(SingleSpace);
												PoDoFo::PdfAnnotation* pAnnotation = pPage->CreateAnnotation(PoDoFo::EPdfAnnotation::ePdfAnnotation_Stamp, rect);
												pAnnotation->SetFlags(PoDoFo::ePdfAnnotationFlags_Print);
												pAnnotation->SetTitle(PoDoFo::PdfString(string("Annuler_p" + to_string(i + 1))));
												pAnnotation->SetAppearanceStream(&xObj);
												painter.FinishPage();
												pFont->SetFontSize(TamponPolice);
											}
										}
										{
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
#ifndef EPR
											painter.DrawLine(TamponS2, 0.0, TamponS2, TamponH1);
#endif

											painter.SetFont(pFont);//Utilise pFont pour écrire...

											painter.SetColor((double)sListeCouleurTranche[t][0], (double)sListeCouleurTranche[t][1], (double)sListeCouleurTranche[t][2]);//Couleur texte format RGB avec 0 à 255 = 0.0 à 1.0
											PoDoFo::PdfString utf8SiteDe(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Site de " + string(NomSite)).c_str()));
											PoDoFo::PdfString utf8Tranche(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Tr. " + to_string(t)).c_str()));
											PoDoFo::PdfString utf8REE(reinterpret_cast<const PoDoFo::pdf_utf8*>(string(strREFERENCEREE).c_str()));
											PoDoFo::PdfString utf8Indice(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Ind. " + string(strINDICEREE)).c_str()));
											PoDoFo::PdfString utf8Folio(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Folio " + to_string(u16_PremierNumero + i - (radioTotalPartiel == 1 ? u16_PageDebut - 1 : 0))).c_str()));
											PoDoFo::PdfString utf8Cycle(reinterpret_cast<const PoDoFo::pdf_utf8*>(string("Cycle " + to_string(t) + string(TrancheCode[t])).c_str()));

											painter.DrawTextAligned(TamponMargL, TamponMargH, TamponS2 - 2 * TamponMargL, utf8Folio, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
#ifndef EPR
											painter.DrawTextAligned(TamponS2 + TamponMargL, TamponMargH, (TamponLargeur - TamponS2) - 2 * TamponMargL, utf8Cycle, PoDoFo::EPdfAlignment::ePdfAlignment_Left);//OK
#endif
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
									}
									MessageOPENCours = fmt::format(u8"Création des tampons {}\nSauvegarde du fichier PDF...", string(txtSpinner));
									MessagePercentOPENCours = fmt::format("Tr.{}", t);
									document.GetInfo()->SetCreator(reinterpret_cast<const PoDoFo::pdf_utf8*>(u8"Procédure traitée par REEMaker"));
									if (FoliotageGenerePDG)
									{
										mPDGHelper.ArrayFromREEMAKER.REErouge = (int)(255.0 * sListeCouleurTranche[t][0]);
										mPDGHelper.ArrayFromREEMAKER.REEvert = (int)(255.0 * sListeCouleurTranche[t][1]);
										mPDGHelper.ArrayFromREEMAKER.REEbleu = (int)(255.0 * sListeCouleurTranche[t][2]);
										mPDGHelper.ArrayFromREEMAKER.ReferenceSite = fileHELPER.utf8_to_ansi(string(NomSite));
										mPDGHelper.ArrayFromREEMAKER.NumeroTranche = std::to_string(t);
										mPDGHelper.ArrayFromREEMAKER.ReferenceREE = fileHELPER.utf8_to_ansi(string(strREFERENCEREE));
										mPDGHelper.ArrayFromREEMAKER.IndiceREE = fileHELPER.utf8_to_ansi(string(strINDICEREE));
										PoDoFo::PdfPage* pPage = document.InsertPage(PoDoFo::PdfRect(0.0, 0.0, 595.0, 842.0), 0);
										PoDoFo::PdfPainter painter;
										painter.SetPage(pPage);

										int NBPageCree = mPDGHelper.DrawOnPage_v2(painter, document);
										painter.FinishPage();
									}
									document.Write(TR_FichierPDFSortie.c_str());
									if (bOpenPDF)
										OuvrePDF(TR_FichierPDFSortie);
								}
								else
								{
									mLogErreur.push_back(TR_FichierPDFSortie);
								}
							}
						}
						catch (const PoDoFo::PdfError& e)
						{
							MY_TRACE("Exception a la ligne %s", e.what());
						}
						catch (exception& ex)
						{
							MY_TRACE("Exception a la ligne %s", ex.what());
						}
						catch (...)
						{
							MY_TRACE("Exception inconnue a la ligne ..");
						}
					}
					LogErreur.clear();
					for (size_t lErr = 0; lErr < mLogErreur.size(); lErr++)
						LogErreur.push_back(mLogErreur[lErr]);
					MessageOPENCours = fmt::format(u8"Fin du foliotage pour toutes les tranches sélectionnées.");
					MessagePercentOPENCours = fmt::format("");
					this_thread::sleep_for(1s);
					AfficheFenetreSpinnerthreadFolioSansPDG = false;
				});
			t.detach();
		}

		if (LogErreur.size() > 1)
			ImGui::OpenPopup(u8"ERREUR##ErreurPDG");
		ImGui::SetNextWindowSize(ImVec2(500.0f, 600.0f));
		if (ImGui::BeginPopupModal(u8"ERREUR##ErreurPDG", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::PushFont(MYFont14bold);
			ImGui::Text(fileHELPER.ConvertWideToUtf8(LogErreur[0]).c_str());
			ImGui::PopFont();
			ImGui::Separator();
			for (size_t i = 1; i < LogErreur.size(); i++)
				ImGui::TextWrapped(fileHELPER.ConvertWideToUtf8(LogErreur[i]).c_str());
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			if (ImGui::Button(u8"Fermer la fenêtre", ImVec2(-1.0f, 24.0f)))
			{
				LogErreur.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}

		if (AfficheModalAvancementFoliotage)
		{
			AfficheModalAvancementFoliotage = false;
			ImGui::OpenPopup("Avancement##TABfolioteruneprocedure");
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

		if (AfficheModalAvancementFoliotageSansPdg)
		{
			AfficheModalAvancementFoliotageSansPdg = false;
			ImGui::OpenPopup("Avancement##threadFolioSansPDG");
		}
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

		if (AfficheModalGenerePageDeGarde)
		{
			AfficheModalGenerePageDeGarde = false;
			ImGui::OpenPopup(u8"Générer pages de gardes en PDF##GenPDF");
		}
		ImGui::SetNextWindowSize(ImVec2(500.0f, 250.0f));
		if (ImGui::BeginPopupModal(u8"Générer pages de gardes en PDF##GenPDF", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text(u8"Sélectionnez la/les tranche(s) :");
			static bool genPDFtranche[9];
			ImGui::Columns(3, "##colTrancheChoice", false);
			for (size_t iT = 0; iT < 10; iT++)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
				ImGui::Checkbox(std::string("##TrancheGenPDF" + to_string(iT)).c_str(), &genPDFtranche[iT]);
				ImGui::PopStyleVar();

				ImGui::SameLine();
				ImGui::Text("Tranche %d", iT);
				ImGui::NextColumn();
			}
			ImGui::Columns(1);
			ImGui::Text(u8"Nom du site pour la page de garde :");
			ImGui::SameLine(250.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::SetNextItemWidth(200.0f);
			ImGui::InputTextWithHint("##NomDuSite", "ex. Flamanville", NomSite_pdg, IM_ARRAYSIZE(NomSite_pdg), 0);
			ImGui::PopStyleVar();
			ImGui::Text(u8"Indice utilisé pour la page de garde :");
			ImGui::SameLine(250.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::SetNextItemWidth(100.0f);
			ImGui::InputTextWithHint("##IndiceProcedure", "ex. A ou PREL", strINDICEREE_pdg, IM_ARRAYSIZE(strINDICEREE_pdg), 0);
			ImGui::PopStyleVar();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			if (ImGui::BeginTable("table##GenPDF", 2, ImGuiTableFlags_None))
			{
				//Decompte des tranches sélectionnées
				bool btnGenPDFactif = false;
				for (size_t iL = 0; iL < 10; iL++)
					if (genPDFtranche[iL])
						btnGenPDFactif = true;

				ImGui::TableNextColumn();
				if (ImGuiAl::Button(u8"Générer la/les page(s) de garde(s)##OKGenPDF", btnGenPDFactif, ImVec2(-1.0f, 22.0f)))
				{
					std::wstring NomPdfTranchebase = L"";
					bool UtilisateurOK = false;
					/*
						Choix du nom du document
					*/
					HRESULT hr;
					CComPtr<IFileSaveDialog> pDlg;
					COMDLG_FILTERSPEC aFileTypes[] = {
						{ L"Fichier PDF Acrobat", L"*.pdf" }
					};
					hr = pDlg.CoCreateInstance(__uuidof(FileSaveDialog));
					if (FAILED(hr))
						return -1;
					pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
					pDlg->SetTitle(L"Générer la/les page(s) de garde(s)");
					pDlg->SetOkButtonLabel(L"&Générer");
					std::wstring defautNom(mPDGHelper.DocumentOuvertWIDE());
					defautNom = defautNom.substr(0, defautNom.find_last_of(L"."));
					pDlg->SetFileName(defautNom.c_str());
					pDlg->SetDefaultExtension(L"pdf");

					hr = pDlg->Show(mHwnd);
					if (SUCCEEDED(hr))
					{
						CComPtr<IShellItem> pItem;
						pDlg->GetResult(&pItem);
						LPOLESTR pwsz = NULL;
						pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
						NomPdfTranchebase = wstring(pwsz);
						CoTaskMemFree(pwsz);
						UtilisateurOK = true;
					}
					else
					{//Pressez sur annuler
						UtilisateurOK = false;
					}
					//Choix validé on foliote, sinon on ne fait rien et on annule
					if (UtilisateurOK)
					{
						LogErreur.clear();
						LogErreur.push_back(L"Les pages de gardes suivantes n'ont pu être créées :");
						for (size_t i = 0; i < 10; i++)
						{
							if (genPDFtranche[i])
							{
								std::wstring CurrentPDGpath = NomPdfTranchebase;
								CurrentPDGpath = CurrentPDGpath.substr(0, CurrentPDGpath.find_last_of(L"."));
								CurrentPDGpath += L"_Tr_" + std::to_wstring(i) + L".pdf";
								bool PeutEtreCree = fileHELPER.JeSuisEcrivable(CurrentPDGpath);
								if (PeutEtreCree)
								{
									PoDoFo::PdfStreamedDocument document(fileHELPER.ConvertWideToANSI(CurrentPDGpath).c_str());
									mPDGHelper.ArrayFromREEMAKER.REErouge = (int)(255.0 * sListeCouleurTranche[i][0]);
									mPDGHelper.ArrayFromREEMAKER.REEvert = (int)(255.0 * sListeCouleurTranche[i][1]);
									mPDGHelper.ArrayFromREEMAKER.REEbleu = (int)(255.0 * sListeCouleurTranche[i][2]);
									mPDGHelper.ArrayFromREEMAKER.NumeroTranche = std::to_string(i);
									mPDGHelper.ArrayFromREEMAKER.ReferenceSite = fileHELPER.utf8_to_ansi(string(NomSite_pdg));
									mPDGHelper.ArrayFromREEMAKER.ReferenceREE = fileHELPER.utf8_to_ansi(string(strREFERENCEREE));
									mPDGHelper.ArrayFromREEMAKER.IndiceREE = fileHELPER.utf8_to_ansi(string(strINDICEREE_pdg));
									PoDoFo::PdfPage* pPage = document.CreatePage(PoDoFo::PdfRect(0.0, 0.0, 595.0, 842.0));
									PoDoFo::PdfPainter painter;
									painter.SetPage(pPage);
									int NBPageCree = mPDGHelper.DrawOnPage_v2(painter, document);
									painter.FinishPage();
									document.Close();
									if (bOpenPDF)
										OuvrePDF(CurrentPDGpath);
								}
								else
								{
									LogErreur.push_back(CurrentPDGpath);
								}
							}
						}

						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::TableNextColumn();
				if (ImGui::Button(u8"Annuler l'opération##CancelGenPDF", ImVec2(-1.0f, 22.0f)))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndTable();
			}
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}

		if (AfficheModalSupprimePageDeGarde)
		{
			AfficheModalSupprimePageDeGarde = false;
			ImGui::OpenPopup(u8"Supprimer la page de garde utilisateur##SuppPDG");
		}
		ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f));
		if (ImGui::BeginPopupModal(u8"Supprimer la page de garde utilisateur##SuppPDG", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text(u8"Supprimer la page de garde utilisateur :");
			ImGui::PushFont(MYFont14bold);
			ImGui::Text(ListePDGUser[item_current_vPDGuser].c_str());
			ImGui::PopFont();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			if (ImGui::BeginTable("table##OuiNonSuppFolio", 2, ImGuiTableFlags_None))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Button(u8"Supprimer##OKDelete", ImVec2(-1.0f, 22.0f)))
				{
					try
					{
						filesystem::remove(CheminPDGuser + fileHELPER.ConvertUtf8ToWide(ListePDGUser[item_current_vPDGuser]));
					}
					catch (const std::exception&)
					{
					}
					//On rafraichit
					ListePDGModele.clear();
					pdgClassSEED = sGenerate(12);
					item_current_vPDG = 0;
					for (const auto& p : filesystem::directory_iterator(CheminPDG))
					{
						string pPathExtension = p.path().extension().string();
						transform(pPathExtension.begin(), pPathExtension.end(), pPathExtension.begin(), ::tolower);
						string pPathFileName = p.path().filename().string();
						transform(pPathFileName.begin(), pPathFileName.end(), pPathFileName.begin(), ::tolower);
						if (pPathExtension == ".txt")
							if (pPathFileName == "page de garde standard.txt")
								ListePDGModele.insert(ListePDGModele.begin() + 0, p.path().filename().u8string());
							else
								ListePDGModele.push_back(p.path().filename().u8string());
					}
					ListePDGUser.clear();
					item_current_vPDGuser = -1;
					for (const auto& p : filesystem::directory_iterator(CheminPDGuser))
					{
						string pPathExtension = p.path().extension().string();
						transform(pPathExtension.begin(), pPathExtension.end(), pPathExtension.begin(), ::tolower);
						string pPathFileName = p.path().filename().string();
						transform(pPathFileName.begin(), pPathFileName.end(), pPathFileName.begin(), ::tolower);
						if (pPathExtension == ".txt")
							ListePDGUser.push_back(p.path().filename().u8string());
					}
					if (item_current_vPDGuser == -1 && ListePDGUser.size() > 0)
						item_current_vPDGuser = 0;
					mPDGHelper.SetBaseModelePath(CheminPDG);
					mPDGHelper.OpenAndParseConfig_v2(fileHELPER.ConvertUtf8ToWide(ListePDGModele[0]));
					TRACE_PDG("Nombre Objet à dessiner : %d\n", mPDGHelper.ItemCount());
					ImGui::CloseCurrentPopup();
				}
				ImGui::TableSetColumnIndex(1);
				if (ImGui::Button(u8"Annuler l'opération##CancelDelete", ImVec2(-1.0f, 22.0f)))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndTable();
			}
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}

		if (AfficheModalAnnulationDeFolio)
		{
			AfficheModalAnnulationDeFolio = false;
			ImGui::OpenPopup("Annulation de folio##AnnulationFolio");
		}
		ImGui::SetNextWindowSize(ImVec2(width - 100.0f, height - 120.0f));
		if (ImGui::BeginPopupModal("Annulation de folio##AnnulationFolio", NULL, ImGuiWindowFlags_NoResize))
		{
			if (isGenereMiniature || isLoadingMiniature)
			{
				//progress
				if (isGenereMiniature)
				{
					//GET NBFiles
					if (tmrGenereMiniature.get_elapsed_ms() > 2000)
					{
						tmrGenereMiniature.start();
						{
							try
							{
								auto dirIter = std::filesystem::directory_iterator(CheminPopplerPDFPPMTempOut);
								GenereMiniaturefileCount = (int)std::count_if(
									begin(dirIter),
									end(dirIter),
									[](auto& entry) { return entry.is_regular_file(); }
								);								//
							}
							catch (const std::exception& e)
							{
								MY_TRACE("Exception a la ligne:  %s", e.what());
							}
						}
						{
							try
							{
								auto dirIter = std::filesystem::directory_iterator(CheminPopplerPDFPPMTempOutMini);
								GenereMiniaturefileCount += (int)std::count_if(
									begin(dirIter),
									end(dirIter),
									[](auto& entry) { return entry.is_regular_file(); }
								);								//
							}
							catch (const std::exception& e)
							{
								MY_TRACE("Exception a la ligne:  %s", e.what());
							}
						}
					}
					ImGui::Text(u8"Génération apercu :");
					ImGui::SameLine(200.0f);
					std::string NBPages = fmt::format(u8"{} apercu(s) générée(s)", (GenereMiniaturefileCount / 2));
					float PCENT = ((100.0f / (float)vecMediaBox.size()) * (float)(GenereMiniaturefileCount / 2)) / 100.0f;
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::ProgressBar(PCENT, ImVec2(200.0f, 22.0f), NBPages.c_str());
					ImGui::PopStyleVar();
				}
				else
				{
					ImGui::Text(u8"Génération apercu :");
					ImGui::SameLine(200.0f);
					std::string NBPages = fmt::format(u8"{} apercu(s) générée(s)", vecMediaBox.size());
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::ProgressBar(100.0f, ImVec2(200.0f, 22.0f), NBPages.c_str());
					ImGui::PopStyleVar();
				}
				if (isLoadingMiniature && !isGenereMiniature)
				{
					ImGui::Text(u8"Miniatures chargées en mémoire : ");
					ImGui::SameLine();
					std::string NBPages = fmt::format(u8"{} miniature(s) chargées(s)", CompteLoading);
					float PCENT = ((100.0f / (float)vecMediaBox.size()) * (float)CompteLoading) / 100.0f;
					ImGui::ProgressBar(PCENT, ImVec2(200.0f, 22.0f), NBPages.c_str());
					bool AllDone = true;
					int CasseLoading = 0;
					for (size_t iDtex = 0; iDtex < vecListeTexture.size(); iDtex++)
					{
						if (!vecListeTexture[iDtex].my_image_success_loading)
						{
							AllDone = false;
							int my_image_width = 0;
							int my_image_height = 0;
							GLuint my_image_texture = 0;
							vecListeTexture[iDtex].my_image_success_loading = LoadTextureFromFile(vecListeTexture[iDtex].Chemin.c_str(), &my_image_texture, &my_image_width, &my_image_height);
							vecListeTexture[iDtex].my_image_width = my_image_width;
							vecListeTexture[iDtex].my_image_height = my_image_height;
							vecListeTexture[iDtex].my_image_texture = my_image_texture;
							CompteLoading++;
							CasseLoading++;
							if (CasseLoading > 60)
								break;
						}
					}
					if (AllDone)
					{
						isLoadingMiniature = false;
						//On precharge la premiere image
						int my_image_width = 0;
						int my_image_height = 0;
						GLuint my_image_texture = 0;
						mApercuTexture.my_image_success_loading = LoadTextureFromFile(vecListeTextureMaxiPathOnly[0].Chemin.c_str(), &my_image_texture, &my_image_width, &my_image_height);
						mApercuTexture.my_image_width = my_image_width;
						mApercuTexture.my_image_height = my_image_height;
						mApercuTexture.my_image_texture = my_image_texture;
						MY_MSG("Chargement Preview avec GluID %d statut %s", mApercuTexture.my_image_texture, mApercuTexture.my_image_success_loading ? "TRUE" : "FALSE");
					}
				}
				else if (isLoadingMiniature && isGenereMiniature)
				{
					ImGui::Text(u8"Miniatures chargées en mémoire : ");
					ImGui::SameLine(200.0f);
					std::string NBPages = fmt::format(u8"{} miniature(s) chargées(s)", 0);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::ProgressBar(0.0f, ImVec2(200.0f, 22.0f), NBPages.c_str());
					ImGui::PopStyleVar();
				}
				else
				{
					ImGui::Text(u8"Miniatures chargées en mémoire  : ");
					ImGui::SameLine(200.0f);
					std::string NBPages = fmt::format(u8"{} miniature(s) chargées(s)", vecMediaBox.size());
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::ProgressBar(100.0f, ImVec2(200.0f, 22.0f), NBPages.c_str());
					ImGui::PopStyleVar();
				}
			}

			if (!isLoadingMiniature && !isGenereMiniature)
			{
				if (ImGui::BeginTable("##tablePreview", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX/*| ImGuiTableFlags_Borders (Rajoute une ligne en trop ??)*/,
					ImVec2(-1.0f, ImGui::GetContentRegionMax().y - 70.0f)))
				{
					ImGui::TableSetupColumn("Liste des miniatures##Actions", ImGuiTableColumnFlags_WidthStretch, 0);
					ImGui::TableSetupColumn("Apercu miniature##Image", ImGuiTableColumnFlags_WidthFixed, 700.0f, 1);
					ImGui::TableHeadersRow();
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					if (ImGui::BeginTable("##tableCKBOX", 1, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg,
						ImVec2(-1.0f, ImGui::GetContentRegionMax().y - 50.0f /*- 100.0f*/)))
					{
						ImGui::TableSetupColumn("ckb##__ckb", ImGuiTableColumnFlags_WidthStretch, 0.0f, 0);
						int  DummyCount = 0;
						for (size_t colLs = 0; colLs < vecListeTexture.size(); colLs++)
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
							ImGui::Checkbox(fmt::format(u8"##Page {}", colLs).c_str(), &FolioProcedureAAnnulerBKUP[colLs]);
							ImGui::SameLine();
							const bool mSelected = false;

							bool InfoIsItemVisible = ImGui::IsItemVisible();
							if (InfoIsItemVisible)
							{
								if (ImGui::Selectable(fmt::format("##selec{}", colLs).c_str(), mSelected, ImGuiSelectableFlags_None, ImVec2(0.0f, 56.0f)))
								{
									if (colLs == MiniatureSelectionnee)
									{
										//Deja chargee
									}
									else
									{
										//On doit chargée
										MiniatureSelectionnee = colLs;
										MY_MSG("Suppression Preview avec GluID %d", mApercuTexture.my_image_texture);
										glDeleteTextures(1, &mApercuTexture.my_image_texture);
										int my_image_width = 0;
										int my_image_height = 0;
										GLuint my_image_texture = 0;
										mApercuTexture.my_image_success_loading = LoadTextureFromFile(vecListeTextureMaxiPathOnly[MiniatureSelectionnee].Chemin.c_str(), &my_image_texture, &my_image_width, &my_image_height);
										mApercuTexture.my_image_width = my_image_width;
										mApercuTexture.my_image_height = my_image_height;
										mApercuTexture.my_image_texture = my_image_texture;
										MY_MSG("Chargement Preview avec GluID %d statut %s", mApercuTexture.my_image_texture, mApercuTexture.my_image_success_loading ? "TRUE" : "FALSE");
									}
								}
								ImGui::SameLine();
								ImGui::Image((void*)(intptr_t)vecListeTexture[colLs].my_image_texture, ImVec2(vecListeTexture[colLs].my_image_width, vecListeTexture[colLs].my_image_height));
								ImGui::PopStyleVar();
								ImGui::SameLine();
#ifdef NDEBUG
								ImGui::Text(fmt::format(u8"Page {}", colLs + 1).c_str());
#else
								ImGui::Text(fmt::format(u8"Page {} (GluID {})", colLs + 1, to_string(vecListeTexture[colLs].my_image_texture)).c_str());
#endif // NDEBUG
							}
							else
							{
								ImGui::PopStyleVar();
								DummyCount++;
								ImGui::Dummy(ImVec2(-1.0f, 64.0f));
							}
						}
						ImGui::EndTable();
					}
					ImGui::TableSetColumnIndex(1);
					static bool ZoomImage = false;
					ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.00f, 0.00f, 0.00f, 0.50f));
					if (ImGui::BeginTable("##tableMaxPV", 1, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg,
						ImVec2(-1.0f, ImGui::GetContentRegionMax().y - 50.0f /*- 100.0f*/)))
					{
						ImGui::TableSetupColumn("MaxPV##__mxPV", ImGuiTableColumnFlags_WidthStretch, 0.0f, 0);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Dummy(ImVec2(2.0f, 10.0f)); ImGui::SameLine();
						ImGui::Image((void*)(intptr_t)mApercuTexture.my_image_texture,
							ImVec2(mApercuTexture.my_image_width * (ZoomImage ? 2 : 1), mApercuTexture.my_image_height * (ZoomImage ? 2 : 1)));
						ImGui::EndTable();
					}
					ImGui::PopStyleColor();

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					if (ImGui::Button(u8"Tout sélectionner"))
						for (size_t ze = 0; ze < 9999; ze++)
							FolioProcedureAAnnulerBKUP[ze] = true;
					ImGui::SameLine();
					if (ImGui::Button(u8"Tout désélectionner"))
						for (size_t ze = 0; ze < 9999; ze++)
							FolioProcedureAAnnulerBKUP[ze] = false;
					ImGui::PopStyleVar();
					ImGui::TableSetColumnIndex(1);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
					ImGui::Checkbox(u8"Zoom x2##Zoom", &ZoomImage);
					ImGui::PopStyleVar();
					ImGui::SameLine();
#ifdef NDEBUG
					ImGui::Text(fmt::format(u8"Apercu de la page {}", MiniatureSelectionnee + 1).c_str());
#else
					ImGui::Text(fmt::format(u8"Apercu de la page {} (GluID {})", MiniatureSelectionnee + 1, to_string(mApercuTexture.my_image_texture)).c_str());
#endif // NDEBUG

					ImGui::EndTable();
				}
			}
			if (ImGui::BeginTable("##tableAnnulationFolio", 2, ImGuiTableFlags_SizingStretchSame, ImVec2(-1.0f, 30.0f)))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
				if (ImGuiAl::Button(u8"Valider le choix des folio(s) à annuler", (!isGenereMiniature && !isLoadingMiniature), ImVec2(-1.0f, 30.0f)))
				{
					for (size_t zz = 0; zz < 9999; zz++)//On transfert cette fenetre dans l'original
						FolioProcedureAAnnuler[zz] = FolioProcedureAAnnulerBKUP[zz];
					MY_MSG("Suppression Preview avec GluID %d", mApercuTexture.my_image_texture);
					glDeleteTextures(1, &mApercuTexture.my_image_texture);
					for (size_t ert = 0; ert < vecListeTexture.size(); ert++)
						glDeleteTextures(1, &vecListeTexture[ert].my_image_texture);
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleVar();

				ImGui::TableSetColumnIndex(1);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
				if (ImGuiAl::Button(u8"Ne pas annuler de folio", true, ImVec2(-1.0f, 30.0f)))
				{
					MY_MSG("Suppression Preview avec GluID %d", mApercuTexture.my_image_texture);
					glDeleteTextures(1, &mApercuTexture.my_image_texture);
					for (size_t ert = 0; ert < vecListeTexture.size(); ert++)
						glDeleteTextures(1, &vecListeTexture[ert].my_image_texture);

					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleVar();
				ImGui::EndTable();
			}
			ImGui::EndPopup();
		}

		if (AfficheModalNomPageDeGarde)
		{
			AfficheModalNomPageDeGarde = false;
			ImGui::OpenPopup(u8"Choix du nom de la page de garde");
		}
		ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f));
		if (ImGui::BeginPopupModal(u8"Choix du nom de la page de garde", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text(u8"Saisir le nom sous lequel sera enregistré la page de garde utilisateur :");
			bool isEnterTEXT = false;
			if (SetEnterTextKeyboardSavePDGtodisk)
			{
				SetEnterTextKeyboardSavePDGtodisk = false;
				ImGui::SetKeyboardFocusHere();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::InputTextWithHint("##txtUnPDGtxtnom", u8"Maximum de 128 caractères", NomPDGtxt, 128, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				SetEnterTextKeyboardSavePDGtodisk = true;
				isEnterTEXT = true;
			}
			if (ImGui::BeginTable("table##SauvePage", 2, ImGuiTableFlags_None))
			{
				ImGui::TableNextColumn();
				if (ImGui::Button(u8"Sauvegarder la page de garde", ImVec2(-1.0f, 22.0f)) || isEnterTEXT)
				{
					mPDGHelper.BurstVersDisque(CheminPDGuser + fileHELPER.ConvertUtf8ToWide(std::string(NomPDGtxt)) + L".txt");
					//On rafraichit
					ListePDGModele.clear();
					pdgClassSEED = sGenerate(12);
					item_current_vPDG = 0;
					for (const auto& p : filesystem::directory_iterator(CheminPDG))
					{
						string pPathExtension = p.path().extension().string();
						transform(pPathExtension.begin(), pPathExtension.end(), pPathExtension.begin(), ::tolower);
						string pPathFileName = p.path().filename().string();
						transform(pPathFileName.begin(), pPathFileName.end(), pPathFileName.begin(), ::tolower);
						if (pPathExtension == ".txt")
							if (pPathFileName == "page de garde standard.txt")
								ListePDGModele.insert(ListePDGModele.begin() + 0, p.path().filename().u8string());
							else
								ListePDGModele.push_back(p.path().filename().u8string());
					}
					ListePDGUser.clear();
					item_current_vPDGuser = -1;
					for (const auto& p : filesystem::directory_iterator(CheminPDGuser))
					{
						string pPathExtension = p.path().extension().string();
						transform(pPathExtension.begin(), pPathExtension.end(), pPathExtension.begin(), ::tolower);
						string pPathFileName = p.path().filename().string();
						transform(pPathFileName.begin(), pPathFileName.end(), pPathFileName.begin(), ::tolower);
						if (pPathExtension == ".txt")
						{
							ListePDGUser.push_back(p.path().filename().u8string());
							if (p.path().filename().wstring() == fileHELPER.ConvertUtf8ToWide(std::string(NomPDGtxt)) + L".txt")
								item_current_vPDGuser = ListePDGUser.size() - 1;
						}
					}
					if (item_current_vPDGuser == -1 && ListePDGUser.size() > 0)
						item_current_vPDGuser = 0;
					mPDGHelper.SetBaseModelePath(CheminPDGuser);
					if (ListePDGUser.size() > 0 && item_current_vPDGuser > -1)
						if (ListePDGUser.size() >= (item_current_vPDGuser + 1))
							mPDGHelper.OpenAndParseConfig_v2(fileHELPER.ConvertUtf8ToWide(ListePDGUser[item_current_vPDGuser]));
					TRACE_PDG("Nombre Objet à dessiner : %d\n", mPDGHelper.ItemCount());
					ImGui::CloseCurrentPopup();
				}
				ImGui::TableNextColumn();
				if (ImGui::Button(u8"Annuler l'opération", ImVec2(-1.0f, 22.0f)))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndTable();
			}
			ImGui::PopStyleVar();
			ImGui::EndPopup();
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
	try
	{
		uintmax_t resRemove = filesystem::remove_all(CheminTemp);
		MY_MSG("Sortie, suppression Session avec %d élément(s)", resRemove);
	}
	catch (const std::exception& e)
	{
		MY_TRACE("Exception a la ligne:  %s", e.what());
	}

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
	if (fCONFIG != NULL)
	{
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
			if (fCONFIG != NULL)
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
		//Nom site PdG
		fwrite(NomSite_pdg, sizeof(unsigned char), 32, fCONFIG);
		//Indice PdG
		fwrite(strINDICEREE_pdg, sizeof(unsigned char), 5, fCONFIG);
		//Flag ouvrePDF après génération
		FILE_write16(fCONFIG, (uint16_t)bOpenPDF);
		fclose(fCONFIG);
	}
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

	if (fCONFIG != NULL)
	{
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
		//Nom site PdG
		fread_s(NomSite_pdg, 32, sizeof(unsigned char), 32, fCONFIG);
		//Indice PdG
		fread_s(strINDICEREE_pdg, 5, sizeof(unsigned char), 5, fCONFIG);
		//Flag ouvrePDF après génération
		bOpenPDF = (bool)FILE_read16(fCONFIG);


		fclose(fCONFIG);

	}
	return true;
}
bool GenereMiniature(int id, int startingPage, int endingPage, std::wstring DocumentPDF) {
	MY_MSG("Lancement Thread %d [bloc %d-%d]", id, startingPage, endingPage);
	FileHelper mPopplerFH(CheminPopplerPDFPPM);
	wstring PathOutputImage = CheminPopplerPDFPPMTempOut + L"\\img_";
	wstring Parametre = L"-f " + to_wstring(startingPage) + L" -l " + to_wstring(endingPage) + L" -q -r 40 -jpeg -jpegopt quality=75,optimize=y \"" + DocumentPDF + L"\" \"" + PathOutputImage + L"\"";
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	wstring wFile = mPopplerFH.RetourneCheminComplet();
	ShExecInfo.lpFile = wFile.c_str();
	ShExecInfo.lpParameters = Parametre.c_str();
	wstring wBasePath = mPopplerFH.RetourneDossier(false);
	ShExecInfo.lpDirectory = wBasePath.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	DWORD exitCDE;
	GetExitCodeProcess(ShExecInfo.hProcess, &exitCDE);
	CloseHandle(ShExecInfo.hProcess);
	MY_MSG("Sortie Thread %d [bloc %d-%d] (Code sortie %d)", id, startingPage, endingPage, exitCDE);
	return true;
}
bool GenereMiniatureMIN(int id, int startingPage, int endingPage, std::wstring DocumentPDF) {
	MY_MSG("Lancement Thread %d [bloc %d-%d]", id, startingPage, endingPage);
	FileHelper mPopplerFH(CheminPopplerPDFPPM);
	wstring PathOutputImage = CheminPopplerPDFPPMTempOutMini + L"\\img_";
	wstring Parametre = L"-f " + to_wstring(startingPage) + L" -l " + to_wstring(endingPage) + L" -q -r 10 -scale-to 64 -jpeg -jpegopt quality=60,optimize=y \"" + DocumentPDF + L"\" \"" + PathOutputImage + L"\"";
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	wstring wFile = mPopplerFH.RetourneCheminComplet();
	ShExecInfo.lpFile = wFile.c_str();
	ShExecInfo.lpParameters = Parametre.c_str();
	wstring wBasePath = mPopplerFH.RetourneDossier(false);
	ShExecInfo.lpDirectory = wBasePath.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	DWORD exitCDE;
	GetExitCodeProcess(ShExecInfo.hProcess, &exitCDE);
	CloseHandle(ShExecInfo.hProcess);
	MY_MSG("Sortie Thread %d [bloc %d-%d] (Code sortie %d)", id, startingPage, endingPage, exitCDE);
	return true;
}
bool OuvrePDF(std::wstring CheminDocPDF)
{
	ShellExecuteW(NULL, L"open", CheminDocPDF.c_str(), NULL, NULL, SW_SHOWNORMAL);
	return true;
}
void MyTrace(int line, const char* fileName, const char* msg, ...)
{
	FileHelper mFH = FileHelper(string(fileName));

	va_list args;
	char buffer[256] = { 0 };
	sprintf_s(buffer, "TRACE: %s(%d) : ", mFH.RetourneNomFichierS().c_str(), line);
	string mMessage(buffer);
	/*OutputDebugStringA(buffer);*/

	// retrieve the variable arguments
	va_start(args, msg);
	vsprintf_s(buffer, msg, args);
	mMessage += string(buffer);
	/*OutputDebugStringA(buffer);
	*/
	va_end(args);
	OutputDebugStringA(mMessage.c_str());
}
void MyMsg(int line, const char* fileName, const char* msg, ...)
{
	FileHelper mFH = FileHelper(string(fileName));

	va_list args;
	char buffer[256] = { 0 };
	sprintf_s(buffer, "MSG: %s(%d) : ", mFH.RetourneNomFichierS().c_str(), line);
	string mMessage(buffer);
	/*OutputDebugStringA(buffer);*/

	// retrieve the variable arguments
	va_start(args, msg);
	vsprintf_s(buffer, msg, args);
	mMessage += string(buffer);
	/*OutputDebugStringA(buffer);
	*/
	va_end(args);
	OutputDebugStringA(mMessage.c_str());
}

#pragma region TestFONT
//void CreateFontSztd()
//{
//	auto dirIter = std::filesystem::directory_iterator(L"c:\\roboto");
//	for (auto& entry : dirIter)
//	{
//		if (entry.is_regular_file())
//		{
//			FileHelper llFH(entry.path().wstring());
//			FILE* lFin = NULL;
//			FILE* lFout = NULL;
//			std::vector<uint8_t> fileIn;
//			uint64_t TailleIn = std::filesystem::file_size(llFH.RetourneCheminComplet());
//			_wfopen_s(&lFin, llFH.RetourneCheminComplet().c_str(), L"rb");
//			if (lFin != NULL)
//			{
//				fileIn.resize(TailleIn);
//				fread_s(&fileIn[0], fileIn.size(), sizeof(uint8_t), fileIn.size(), lFin);
//				fclose(lFin);
//			}
//			std::vector<uint8_t> fileOut;
//			CompressAndCryptChunk(fileIn, fileOut, 22, "clé", "iv");
//			fileIn.clear();
//			_wfopen_s(&lFout, std::wstring(llFH.RetourneCheminComplet() + L".zstd").c_str(), L"wb");
//			if (lFout != NULL)
//			{
//				fwrite(&fileOut[0], sizeof(uint8_t), fileOut.size(), lFout);
//				fclose(lFout);
//			}
//			fileOut.clear();
//		}
//	}
//}
//
//std::vector<uint8_t> GetFontFromResource(HINSTANCE hInstance, int IDResource, std::wstring TypeResource)
//{
//	std::vector<uint8_t> vecDecomp;
//	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(IDResource), TypeResource.c_str());
//
//	if (hResource)
//	{
//		HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
//		if (hLoadedResource)
//		{
//			LPVOID pLockedResource = LockResource(hLoadedResource);
//			if (pLockedResource)
//			{
//				DWORD dwResourceSize = SizeofResource(hInstance, hResource);
//				if (0 != dwResourceSize)
//				{
//					std::vector<uint8_t> vecRead;
//					vecRead.resize(dwResourceSize);
//					memcpy_s(&vecRead[0], dwResourceSize, pLockedResource, dwResourceSize);
//
//					uint32_t TailleComp = MEM_read32(&vecRead[0]);
//					uint32_t TailleUncomp = MEM_read32(&vecRead[4]);
//					uint64_t TailleHASH = MEM_read64(&vecRead[8]);
//					vecRead.erase(vecRead.begin(), vecRead.begin() + 16);
//					DecompressCryptChunk(vecRead, vecDecomp, TailleComp, TailleUncomp, TailleHASH, "clé", "iv");
//					UnlockResource(hLoadedResource);
//					//FreeResource(pLockedResource);
//					return vecDecomp;
//
//					// Use pLockedResource and dwResourceSize however you want
//				}
//			}
//		}
//	}
//	return vecDecomp;
//}
#pragma endregion
#pragma region CodeSaveOpen
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
				// Save the file here, 'pwsz' has the full path
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
#pragma endregion

/*
*    *-*-*-*-*-*
*   * 𝕋 𝕆 𝔻 𝕆 *
*    *-*-*-*-*-*
*/
