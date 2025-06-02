#pragma once
#include "../BaseObject/BaseObject.h"

#include <Loaders/Json/JsonManager.h>

class Drip : public BaseObject
{
public:
	~Drip() override;

	void Initialize(Camera* camera) override;

	void Update() override;

	void Draw() override;

private:

	void InitJson();
private:

	std::unique_ptr<JsonManager> jsonManager_;
};

