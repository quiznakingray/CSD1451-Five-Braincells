#include "PlayerStats.h"
#include "SaveManager.h"
#include "AudioManager.h"
#include "GameStateManager.h"
#include "HUD.h"
#include <iostream>

float PlayerStats::GetAttackCooldown() const
{
    float cooldown = BASE_COOLDOWN * (1.0f - proficiency);
    return cooldown < MIN_COOLDOWN ? MIN_COOLDOWN : cooldown;
}

float PlayerStats::GetMaxShieldDuration() const
{
    return BASE_SHIELD_DURATION + proficiency * SHIELD_DURATION_SCALE;
}

bool PlayerStats::RegenStamina(double dt)
{
    int cap = maxJumpStamina;
    if (jumpStamina >= cap)
    {
        jumpStaminaTimer = 0.0f;
        return false;
    }
    jumpStaminaTimer += STAMINA_REGEN_RATE * static_cast<f32>(dt);

    if (jumpStaminaTimer >= STAMINA_REGEN_DURATION)
    {
        jumpStamina++;
        jumpStaminaTimer = 0.0f;
    }
    if (jumpStamina > cap)
        jumpStamina = cap;

    return true;
}

bool PlayerStats::ConsumeJumpStamina()
{
    if (jumpStamina == 0) return false;
    --jumpStamina;
    return true;
}

void PlayerStats::ReducePlayerHealth(int amount)
{
    health-= amount;
    if (health > 0) {
        AudioManager::GetInstance().PlaySFX("playerHurt");
    }
    else
    {
        AudioManager::GetInstance().PlaySFX("playerDie");
        IncreaseDeathCounter();
		health = PlayerStats::GetInstance().maxHealth; // reset health on death
        SaveManager::GetInstance().SavePlayerTime(totalSeconds);
        
        //if (deathCount >= 10)
        //{
        //    HUD::GetInstance().ShowDeathPanel();
        //}
    }
}

void PlayerStats::IncreasePlayerHealth(int amount)
{
    health += amount;
    if (health > maxHealth) health = maxHealth;
    std::cout << "Health: " << health << '\n';
}

int PlayerStats::GetPlayerHealth() const
{
    return health;
}

int PlayerStats::GetPlayerMaxHealth() const
{
    return maxHealth;
}

void PlayerStats::SetPlayerHealth(int h)
{
    health += h;
    if (h < 0) {
        if (health > 0) {
            AudioManager::GetInstance().PlaySFX("playerHurt");
        }
        else
        {
            AudioManager::GetInstance().PlaySFX("playerDie");
            IncreaseDeathCounter();
            SaveManager::GetInstance().SavePlayerTime(totalSeconds);
            if (deathCount < 10) {
                SaveManager::GetInstance().SaveHighScore(highScore);
            }
            //HUD::GetInstance().ShowDeathPanel();
            SaveManager::GetInstance().toContinue = true;
            //GAME_STATE_TYPE respawnLevel = SaveManager::GetInstance().mapSaveData.savedLevel;
            //current = GAME_STATE_TYPE::MENU;
            //next = respawnLevel;
        }
    }
    else {
        if (health > maxHealth)
            health = maxHealth;
    }
}

void PlayerStats::SetPlayerMaxHealth(int mh)
{
    maxHealth = mh;
    if (health > maxHealth) health = maxHealth;
}

void PlayerStats::IncreasePlayerDamage(int amount)
{
    damage += amount;
    if (damage < 0) damage = 0;
    std::cout << "Damage: " << damage << '\n';
}

void PlayerStats::DecreasePlayerDamage(int amount)
{
    damage -= amount;
    if (damage < 0) damage = 0;
}

int PlayerStats::GetPlayerDamage() const
{
    return damage;
}

void PlayerStats::SetPlayerDamage(int d)
{
    damage = d < 0 ? 0 : d;
}

void PlayerStats::IncreasePlayerProficiency(float amount)
{
    proficiency += amount;
    if (proficiency > 1.0f) proficiency = 1.0f;
    if (proficiency < 0.0f) proficiency = 0.0f;
    std::cout << "Proficiency: " << proficiency << '\n';
}

void PlayerStats::DecreasePlayerProficiency(float amount)
{
    proficiency -= amount;
    if (proficiency < 0.0f) proficiency = 0.0f;
}

float PlayerStats::GetPlayerProficiency() const
{
    return proficiency;
}

void PlayerStats::IncreaseDeathCounter()
{
    deathCount++;
    SaveManager::GetInstance().SaveDeathCount(deathCount);
}

void PlayerStats::SetTotalSeconds(float seconds)
{
    totalSeconds = seconds;
}

float PlayerStats::GetTotalSeconds() const
{
    return totalSeconds;
}

void PlayerStats::SetPlayerType(PLAYER_TYPE type)
{
    playerType = type;
}

PLAYER_TYPE PlayerStats::GetPlayerType()
{
    return playerType;
}

//PlayerStats& PlayerStats::Get()
//{
//    static PlayerStats instance;
//    return instance;
//}

void PlayerStats::ResetAll()
{
    health = 5;
    maxHealth = 5;
    damage = 1;
    proficiency = 0.0f;
    speedMult = 1.0f;
    maxJumpStamina = BASE_MAX_STAMINA;
    jumpStamina = BASE_MAX_STAMINA;
    deathCount = 0;
    killCount = 0;
    totalSeconds = 0;
}

void PlayerStats::ResetHealthStamina() {
    health = maxHealth;
    maxJumpStamina = BASE_MAX_STAMINA;
    jumpStamina = BASE_MAX_STAMINA;
}

void PlayerStats::ResetCounters()
{
    deathCount = 0;
    killCount = 0;
    totalSeconds = 0;
}