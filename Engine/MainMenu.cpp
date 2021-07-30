#include "MainMenu.h"
Stage::MainMenu::MainMenu(const GameStageModel base_class, int &high_score):
	GameStage(base_class.wnd_, base_class.gfx_, base_class.stage_, base_class.time_),
	game_title("game_title.bmp"),
	high_score_label(new Label(gfx, "High Score: " + std::to_string(high_score), Vec2{}, Color{ 255,255,255 }, 1.0, Font::Find("pixel"))),
	game_version_label(new Label(gfx, "v" + wstrtostr(wnd.GetVersion()), Vec2{}, Color{ 255,255,255 }, 1.0, Font::Find("pixel"))),
	host(true), multiplayer(false) {
#ifdef _DEBUG
	debug_label = new Label(gfx, "DEBUG", Vec2{}, Color{ 255,255,255 }, 1.0, Font::Find("pixel"));
#endif
	menu_button.resize(menu_button_count);
	menu_button[0] = new Button(gfx, wnd.mouse, Vec2{ (Settings::GameScreenWidth / 2.0) - (200 / 2.0), 325 }, { 200, 50 }, 4);
	menu_button[0]->CreateLabel("Start Game",	Color{ 0,255,255 }, 1.0, Font::Find("pixel"));
	menu_button[1] = new Button(gfx, wnd.mouse, Vec2{ (Settings::GameScreenWidth / 2.0) - (200 / 2.0) - 220, 325 }, { 200, 50 }, 4);
	menu_button[1]->CreateLabel("Test Room", Color{ 0,255,255 }, 1.0, Font::Find("pixel"));
	menu_button[2] = new Button(gfx, wnd.mouse, Vec2{ (Settings::GameScreenWidth / 2.0) - (200 / 2.0), 400 }, { 200, 50 }, 4);
	menu_button[2]->CreateLabel("Multiplayer",	Color{ 0,255,255 }, 1.0, Font::Find("pixel"));
	menu_button[3] = nullptr;
	menu_button[4] = nullptr;
	menu_button[5] = new Button(gfx, wnd.mouse, Vec2{ (Settings::GameScreenWidth / 2.0) - (200 / 2.0), 475 }, { 200, 50 }, 4);
	menu_button[5]->CreateLabel("Exit", Color{ 0,255,255 }, 1.0, Font::Find("pixel"));
	high_score_label->SetPos(Vec2{ (Settings::GameScreenWidth - high_score_label->GetWidth()) / 2, 550 });
	if (debug_label != nullptr) debug_label->SetPos(Vec2{ 10, 10 });
	game_version_label->SetPos(Vec2{ Settings::GameScreenWidth - game_version_label->GetWidth() - 4, Settings::GameScreenHeight - game_version_label->GetHeight() });
	updates_textlist = new TextList(wnd.mouse, HitboxD({ 10, 470 }, { 520, 710 }));
	std::vector<std::string> updates_textvector;
	for (int i = 1; ; i++) {
		std::string text = fgetstr("updates_description.txt", i);
		if (text != std::string()) updates_textvector.push_back(text);
		else break;
	}
	for (std::string text : updates_textvector) {
		if (text == "!next") text = "";
		updates_textlist->Add(new Label(gfx, text, { 0, 0 }, { 255, 255, 255 }, 0.7, Font::Find("pixel")));
	}
}
Stage::MainMenu::~MainMenu() {
	for (int i = 0; i < menu_button_count; i++) {
		if (menu_button[i] != nullptr) { delete menu_button[i]; menu_button[i] = nullptr; }
	}
	if (high_score_label != nullptr) { delete high_score_label; high_score_label = nullptr; }
	if (debug_label != nullptr) { delete debug_label; debug_label = nullptr; }
	if (updates_textlist != nullptr) { delete updates_textlist; updates_textlist = nullptr; }
	if (last_update_description_label != nullptr) { delete last_update_description_label; last_update_description_label = nullptr; }
	if (game_version_label != nullptr) { delete game_version_label; game_version_label = nullptr; }
}
void Stage::MainMenu::Update() {
	for (int i = 0; i < menu_button_count; i++) {
		if (menu_button[i] != nullptr) {
			menu_button[i]->Update();
			if (menu_button[i]->MouseCollision(Mouse::Event::LPress)) {
				if (i == 0) {
					multiplayer = false;
					host = true;
					stage = GameStageList::StageName::GameProcess;
					break;
				}
				else if (i == 1) {
					stage = GameStageList::StageName::TestRoom;
					break;
				}
				else if (i == 2) {
					if (!menu_button_mp_active) {
						menu_button[3] = new Button(gfx, wnd.mouse, Vec2{ (Settings::GameScreenWidth - 200) / 2 - 220, 400 }, { 200, 50 }, 4);
						menu_button[3]->CreateLabel("Client", Color{ 0,255,255 }, 1.0, Font::Find("pixel"));
						menu_button[4] = new Button(gfx, wnd.mouse, Vec2{ (Settings::GameScreenWidth - 200) / 2 + 220, 400 }, { 200, 50 }, 4);
						menu_button[4]->CreateLabel("Server", Color{ 0,255,255 }, 1.0, Font::Find("pixel"));
						menu_button_mp_active = true;
					}
					else {
						delete menu_button[3]; menu_button[3] = nullptr;
						delete menu_button[4]; menu_button[4] = nullptr;
						menu_button_mp_active = false;
					}
					break;
				}
				else if (i == 3) {
					multiplayer = true;
					host = false;
					stage = GameStageList::StageName::GameProcess;
					break;
				}
				else if (i == 4) {
					multiplayer = true;
					host = true;
					stage = GameStageList::StageName::GameProcess;
					break;
				}
				else if (i == 5) {
					wnd.Kill();
					break;
				}
			}
		}
	}
	updates_textlist->Update();
}
void Stage::MainMenu::Draw() {
	gfx.DrawSprite(
		{ (Settings::GameScreenWidth - game_title.GetWidth()) / 2, 50 },
		game_title,
		SpriteEffect::Substitution{ Color{ 255,255,255 }, Color{ 255,0,255 } }
	);
	for (int i = 0; i < menu_button_count; i++) {
		if (menu_button[i] != nullptr) { menu_button[i]->Redraw(); menu_button[i]->Draw(); }
	}
	high_score_label->Draw();
	if (debug_label != nullptr) debug_label->Draw();
	//last_update_description_label->Draw();
	updates_textlist->Draw();
	game_version_label->Draw();
}
std::string Stage::MainMenu::GetInfo() const {
	std::string info;
	// bool multiplayer
	info += std::to_string(multiplayer); info += " ";
	// bool host
	info += std::to_string(host); info += " ";
	return info;
}