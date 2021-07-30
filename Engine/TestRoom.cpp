#include "TestRoom.h"
Stage::TestRoom::TestRoom(const GameStageModel base_class):
	GameStage(base_class.wnd_, base_class.gfx_, base_class.stage_, base_class.time_),
	exit_button(new Button(gfx, wnd.mouse, Vec2{ 10, Settings::GameScreenHeight - 60 }, { 200, 50 }, 4)) {
	exit_button->CreateLabel("Back to Menu", Color{ 0,255,255 }, 0.5, Font::Find("pixel"));
	experiment_button.resize(experiment_button_count);
	experiment_button[0] = new Button(gfx, wnd.mouse, Vec2{ 10, 10 + (50 + 10) * 0 }, { 200, 50 }, 4);
	experiment_button[0]->CreateLabel("Clear", Color{ 0,255,255 }, 0.9, Font::Find("pixel"));
	experiment_button[1] = new Button(gfx, wnd.mouse, Vec2{ 10, 10 + (50 + 10) * 1 }, { 200, 50 }, 4);
	experiment_button[1]->CreateLabel("Hitbox", Color{ 0,255,255 }, 0.9, Font::Find("pixel"));
}
Stage::TestRoom::~TestRoom() {
	// Common objects
	if (exit_button != nullptr) { delete exit_button; exit_button = nullptr; }
	for (auto button : experiment_button) if (button != nullptr) { delete button; button = nullptr; }
	for (auto button : experiment_addition_button) if (button != nullptr) { delete button; button = nullptr; }
	// // Experiment objects
	if (experiment == Experiment::None) {}
	else if (experiment == Experiment::Hitbox) {
		if (ball != nullptr) { delete ball; ball = nullptr; }
		if (second_ball != nullptr) { delete second_ball; second_ball = nullptr; }
		if (brick != nullptr) { delete brick; brick = nullptr; }
		if (hitbox_label != nullptr) { delete hitbox_label; hitbox_label = nullptr; }
	}
}
void Stage::TestRoom::Update() {
	// Common updates
	mouse_last = mouse_now;
	mouse_now = GameStage::wnd.mouse.GetPos();
	if (exit_button != nullptr) exit_button->Update();
	if (exit_button->MouseCollision(Mouse::Event::LPress)) stage = GameStageList::StageName::MainMenu;
	for (int i = 0; i < experiment_addition_button.size(); i++) {
		if (experiment_addition_button[i] != nullptr) experiment_addition_button[i]->Update();
		else continue;
		if (experiment_addition_button[i]->MouseCollision(Mouse::Event::LPress)) {
			if (experiment = Experiment::None) {}
			else if (experiment = Experiment::Hitbox) {
				// Enable/Disable drawing collision lines
				if (i == 0) {
					if (experiment_addition_lines) {
						experiment_addition_lines = false;
						Settings::DrawCollisionLines() = false;
						experiment_addition_button[i]->UpdateLabel("Lines: No");
					}
					else {
						experiment_addition_lines = true;
						Settings::DrawCollisionLines() = true;
						experiment_addition_button[i]->UpdateLabel("Lines: Yes");
					}
				}
			}
		}
	}
	for (int i = 0; i < experiment_button.size(); i++) {
		if (experiment_button[i] != nullptr) experiment_button[i]->Update();
		else continue;
		if (experiment_button[i]->MouseCollision(Mouse::Event::LPress)) {
			// Clear
			if (i == 0) experiment = Experiment::None;
			// Hitbox
			else if (i == 1) experiment = Experiment::Hitbox;
		}
	}
	// Change experiment
	if (experiment != last_experiment) {
		// Clear last experiment
		if (last_experiment == Experiment::None) {}
		else if (last_experiment == Experiment::Hitbox) {
			if (ball != nullptr) { delete ball; ball = nullptr; }
			if (second_ball != nullptr) { delete second_ball; second_ball = nullptr; }
			if (brick != nullptr) { delete brick; brick = nullptr; }
			if (hitbox_label != nullptr) { delete hitbox_label; hitbox_label = nullptr; }
			for (auto button : experiment_addition_button) if (button != nullptr) { delete button; button = nullptr; }
		}
		last_experiment = experiment;
		// Create new experiment
		if (experiment == Experiment::None) {
			// Addition buttons
			experiment_addition_button.resize(0);
		}
		else if (experiment == Experiment::Hitbox) {
			// Labels
			hitbox_label = new Label(gfx, "Hitbox:", { 220, 10 }, {255, 255, 255}, 1.0, Font::Find("pixel"));
			// Objects
			std::vector<Player*> players;
			ball = new Ball(gfx, players, (Keyboard*)nullptr, (Player*)nullptr,
				{ (double)Settings::GameScreenWidth * 0.65 - Ball::StartWidth * 12.0 / 2, (double)Settings::GameScreenHeight * 0.6 - Ball::StartHeight * 6.0 / 2 },
				{ 12.0, 6.0 }, 0.0, 0.0, "big_ball.bmp");
			second_ball = new Ball(gfx, players, (Keyboard*)nullptr, (Player*)nullptr,
				{ (double)Settings::GameScreenWidth * 0.35 - Ball::StartWidth * 16.0 / 2, (double)Settings::GameScreenHeight * 0.6 - Ball::StartHeight * 24.0 / 2 },
				{ 16.0, 24.0 }, 0.0, 0.0, "big_ball.bmp");
			brick = new Brick(gfx, { (double)Settings::GameScreenWidth * 0.55 - Ball::StartWidth * 10.0 / 2, (double)Settings::GameScreenHeight * 0.3 - Ball::StartHeight * 10.0 / 2 },
				{10.0, 10.0}, *ball, Brick::Type::Common, false);
			// Addition buttons
			experiment_addition_button.resize(1);
			experiment_addition_button[0] = new Button(gfx, wnd.mouse, 
				Vec2{ Settings::GameScreenWidth - 200 - 10, 10 + (50 + 10) * 0 },
				{ 200, 50 }, 4);
			experiment_addition_button[0]->CreateLabel(std::string("Lines: ") + ((experiment_addition_lines) ? std::string("Yes") : std::string("No")), Color{ 0,255,255 }, 0.9, Font::Find("pixel"));
		}
	}
	// Update experiment
	if (experiment == Experiment::None) {}
	else if (experiment == Experiment::Hitbox) {
		ball->Update();
		second_ball->Update();
		brick->Update();
		Mouse &mouse = GameStage::wnd.mouse;
		if (mouse_hold || ball->GetHitbox().GetGeneralHitbox().IsContain(mouse.GetPos()) && mouse.Read().GetType() == Mouse::Event::LPress) {
			if (!mouse_hold) {
				mouse_hold = true;
				attach_pos = ball->GetHitbox().GetPos() - mouse.GetPos();
			}
			if (mouse.Read().GetType() == Mouse::Event::LRelease) {
				mouse_hold = false;
			}
			else {
				ball->GetHitbox().SetPos(mouse.GetPos() + attach_pos);
			}
		}
	}
}
void Stage::TestRoom::Draw() {
	// Common objects
	if (exit_button != nullptr) { exit_button->Redraw(); exit_button->Draw(); }
	for (int i = 0; i < experiment_button_count; i++) {
		if (experiment_button[i] != nullptr) { experiment_button[i]->Redraw(); experiment_button[i]->Draw(); }
	}
	// Experiment objects
	if (experiment == Experiment::None) {}
	else if (experiment == Experiment::Hitbox) {
		second_ball->Draw();
		brick->Draw();
		ball->Draw();
		hitbox_label->Update(std::string("Ellipse - IsTouch: ") + (ball->GetHitbox().IsTouch(second_ball->GetHitbox(), true) ? "Yes" : "No")
			+ std::string("\nEllipse - IsInside: ") + (ball->GetHitbox().IsInside(second_ball->GetHitbox(), true) ? "Yes" : "No")
			+ std::string("\nEllipse - Angle: ") + std::to_string(90.0 / acos(0) * Vec2::GetAngle(ball->GetHitbox().GetCenter(), second_ball->GetHitbox().GetCenter()))
			+ std::string("\nRect - IsTouch: ") + (ball->GetHitbox().IsTouch(brick->GetHitbox(), true) ? "Yes" : "No")
			+ std::string("\nRect - IsInside: ") + (ball->GetHitbox().IsInside(brick->GetHitbox(), true) ? "Yes" : "No")
			+ std::string("\nRect - Angle: ") + std::to_string(90.0 / acos(0) * Vec2::GetAngle(ball->GetHitbox().GetCenter(), brick->GetHitbox().GetCenter())));
		hitbox_label->Draw();
		for (Button *button : experiment_addition_button) { button->Redraw(); button->Draw(); }
	}
}
std::string Stage::TestRoom::GetInfo() const {
	return std::string();
}