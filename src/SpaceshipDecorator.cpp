#include "SpaceshipDecorator.h"

SpaceshipDecorator::SpaceshipDecorator(std::shared_ptr<Spaceship> ship) 
    : Spaceship(ship->GetName(), ship->GetPosition(), ship->GetMaxHp(), ship->GetDamage(), ship->GetArmor(), ship->GetMoveSpeed(), 0, 0, 0, 0),
      coreShip(ship) {}

float SpaceshipDecorator::GetHp() const { return coreShip->GetHp(); }
float SpaceshipDecorator::GetMaxHp() const { return coreShip->GetMaxHp(); }
float SpaceshipDecorator::GetDamage() const { return coreShip->GetDamage(); }
float SpaceshipDecorator::GetArmor() const { return coreShip->GetArmor(); }
float SpaceshipDecorator::GetMoveSpeed() const { return coreShip->GetMoveSpeed(); }
float SpaceshipDecorator::GetCritChance() const { return coreShip->GetCritChance(); }
float SpaceshipDecorator::GetCritDamage() const { return coreShip->GetCritDamage(); }
float SpaceshipDecorator::GetMaxMana() const { return coreShip->GetMaxMana(); }
float SpaceshipDecorator::GetCurrentMana() const { return coreShip->GetCurrentMana(); }
float SpaceshipDecorator::GetAttackSpeed() const { return coreShip->GetAttackSpeed(); }
int SpaceshipDecorator::GetLevel() const { return coreShip->GetLevel(); }
void SpaceshipDecorator::SetLevel(int newLevel) { coreShip->SetLevel(newLevel); }
float SpaceshipDecorator::GetCurrentExp() const { return coreShip->GetCurrentExp(); }
float SpaceshipDecorator::GetMaxExp() const { return coreShip->GetMaxExp(); }
std::string SpaceshipDecorator::GetName() const { return coreShip->GetName(); }
Rectangle SpaceshipDecorator::GetHitbox() const { return coreShip->GetHitbox(); }

void SpaceshipDecorator::TakeDamage(float incomingDamage) { coreShip->TakeDamage(incomingDamage); }

void SpaceshipDecorator::AddObserver(IObserver* observer) { coreShip->AddObserver(observer); }
void SpaceshipDecorator::RemoveObserver(IObserver* observer) { coreShip->RemoveObserver(observer); }
void SpaceshipDecorator::Notify(EventType event, const std::string& data) { coreShip->Notify(event, data); }

void SpaceshipDecorator::SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior) { coreShip->SetShootingBehavior(std::move(behavior)); }
void SpaceshipDecorator::SetWeapon(const std::string& weaponName) { coreShip->SetWeapon(weaponName); }
std::string SpaceshipDecorator::GetWeapon() const { return coreShip->GetWeapon(); }
void SpaceshipDecorator::ReloadStatsFromCSV() { coreShip->ReloadStatsFromCSV(); }
void SpaceshipDecorator::TriggerRecoil() { coreShip->TriggerRecoil(); }
float SpaceshipDecorator::GetRecoilOffset() const { return coreShip->GetRecoilOffset(); }
float SpaceshipDecorator::GetThrusterIntensity() const { return coreShip->GetThrusterIntensity(); }
float SpaceshipDecorator::GetThrusterTiltX() const { return coreShip->GetThrusterTiltX(); }
float SpaceshipDecorator::GetThrusterLengthMult() const { return coreShip->GetThrusterLengthMult(); }
void SpaceshipDecorator::Fire() { coreShip->Fire(); }
void SpaceshipDecorator::GainExp(float amount) { coreShip->GainExp(amount); }
void SpaceshipDecorator::GainMana(float amount) { coreShip->GainMana(amount); }
void SpaceshipDecorator::LevelUp() { coreShip->LevelUp(); }
void SpaceshipDecorator::Heal(float amount) { coreShip->Heal(amount); }

void SpaceshipDecorator::AddArgument(int argId) { coreShip->AddArgument(argId); }
bool SpaceshipDecorator::HasArgument(int argId) const { return coreShip->HasArgument(argId); }
void SpaceshipDecorator::AddPermanentDamage(float amt) { coreShip->AddPermanentDamage(amt); }
float SpaceshipDecorator::GetPermanentDamageBonus() const { return coreShip->GetPermanentDamageBonus(); }

void SpaceshipDecorator::Init() { coreShip->Init(); }
void SpaceshipDecorator::Update(float deltaTime) { coreShip->Update(deltaTime); }
void SpaceshipDecorator::Draw() { coreShip->Draw(); }
void SpaceshipDecorator::Die() { coreShip->Die(); }
