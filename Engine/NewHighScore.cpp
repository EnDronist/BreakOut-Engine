#include "NewHighScore.h"
Stage::NewHighScore::NewHighScore(const GameStageModel base_class, int &high_score):
	GameStage(base_class.wnd_, base_class.gfx_, base_class.stage_, base_class.time_),
	new_high_score_panel(new Button(gfx, wnd.mouse, Vec2{ ((Settings::GameScreenWidth - 400) / 2), ((Settings::GameScreenHeight - 300) / 2) }, { 400, 300 }, 8)),
	new_high_score_label(new Label(gfx, "New High Score: " + std::to_string(high_score), Vec2{}, Color{ 255,255,255 }, 1.0, Font::Find("pixel"))),
	new_high_score_button(new Button(gfx, wnd.mouse, Vec2{
		new_high_score_panel->GetPos().x + ((new_high_score_panel->GetWidth() - 240) / 2),
		new_high_score_panel->GetPos().y + static_cast<int>(new_high_score_panel->GetHeight() * 0.66) - (50 / 2) },
		{ 240, 50 }, 5))
{
	new_high_score_label->SetPos(Vec2{
		new_high_score_panel->GetPos().x + ((new_high_score_panel->GetWidth() - new_high_score_label->GetWidth()) / 2),
		new_high_score_panel->GetPos().y + static_cast<int>(new_high_score_panel->GetHeight() * 0.33) - (new_high_score_label->GetHeight() / 2) }
	);
	new_high_score_button->CreateLabel("Back to Menu", Color{ 255,255,255 }, 1.0, Font::Find("pixel"));
}
Stage::NewHighScore::~NewHighScore() {
	if (new_high_score_panel != nullptr) { delete new_high_score_panel; new_high_score_panel = nullptr; }
	if (new_high_score_label != nullptr) { delete new_high_score_label; new_high_score_label = nullptr; }
	if (new_high_score_button != nullptr) { delete new_high_score_button; new_high_score_button = nullptr; }
}
void Stage::NewHighScore::Update() {
	new_high_score_button->Update();
	if (new_high_score_button->MouseCollision(Mouse::Event::LPress)) {
		stage = GameStageList::StageName::MainMenu;
		return;
	}
}
void Stage::NewHighScore::Draw() {
	new_high_score_panel->Redraw(); new_high_score_panel->Draw();
	new_high_score_label->Draw();
	new_high_score_button->Redraw(); new_high_score_button->Draw();
}