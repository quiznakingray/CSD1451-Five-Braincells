#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H
#include "SaveManager.h"
#include <iostream>
#include "AudioManager.h"
#include "GameStateManager.h"

struct PlayerStats
{

    // base constants
    static constexpr float BASE_COOLDOWN = 0.5f;
    static constexpr float MIN_COOLDOWN = 0.05f;
    static constexpr float BASE_SHIELD_DURATION = 2.0f;
    static constexpr float SHIELD_DURATION_SCALE = 5.0f; // extra seconds added at max proficiency

    static constexpr int   BASE_MAX_STAMINA = 3;     // starting cap
    static constexpr int   HARD_MAX_STAMINA = 5;     // powerup ceiling
    static constexpr float STAMINA_REGEN_RATE = 1.0f;  // charges per second
    static constexpr float STAMINA_REGEN_DURATION = 3.0f;  // charges per second

    // upgradeable stats
    int health = 3;       // current hearts
    int maxHealth = 3;       // maximum hearts
    int damage = 1; // damage dealt per hit
    float proficiency = 0.0f;    // higher = faster cooldowns for bow/longer duration for shield (value between 0 - 1)
    float speedMult = 1.0f;    // movement speed multiplier (1.0 = base)

    // jumping stamina
    int   maxJumpStamina = BASE_MAX_STAMINA;
    int jumpStamina = BASE_MAX_STAMINA;
    double jumpStaminaTimer = 0.0f;

    // counters
    int deathCount = 0;
    int killCount = 0;

    // helpers
    // seconds betw arrow shots
    float GetAttackCooldown() const
    {
        float cooldown = BASE_COOLDOWN * (1.0f - proficiency);
        return cooldown < MIN_COOLDOWN ? MIN_COOLDOWN : cooldown;
    }

    float GetMaxShieldDuration() const
    {
        return BASE_SHIELD_DURATION + proficiency * SHIELD_DURATION_SCALE;
    }
    bool RegenStamina(double dt)
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

    bool ConsumeJumpStamina()
    {
        if (jumpStamina == 0) return false;
        --jumpStamina;
        return true;
    }

    void ReducePlayerHealth()
    {
        health--;
        if (health > 0) {
            AudioManager::GetInstance().PlaySFX("playerHurt");
        }
        else
        {
            AudioManager::GetInstance().PlaySFX("playerDie");
            health = maxHealth;
            SaveManager::GetInstance().toContinue = true;
            GAME_STATE_TYPE respawnLevel = SaveManager::GetInstance().mapSaveData.savedLevel;
            current = GAME_STATE_TYPE::MENU;  // force inner loop to exit
            next = respawnLevel;           // reload the saved level
        }
    }

    void IncreasePlayerHealth(int amount = 1)
    {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << "Health: " << health << '\n';
    }

    int GetPlayerHealth() const {
        return health;
    }

    int GetPlayerMaxHealth() const {
        return maxHealth;
    }

    void SetPlayerHealth(int h) {
        health = h;
        if (health > maxHealth) health = maxHealth;
        if (health < 0) health = 0;
    }

    void SetPlayerMaxHealth(int mh) {
        maxHealth = mh;
        if (health > maxHealth) health = maxHealth;
    }

    // Damage helpers (new)
    void IncreasePlayerDamage(int amount = 1)
    {
        damage += amount;
        if (damage < 0) damage = 0;
        std::cout << "Damage: " << damage << '\n';
    }

    void DecreasePlayerDamage(int amount = 1)
    {
        damage -= amount;
        if (damage < 0) damage = 0;
    }

    int GetPlayerDamage() const {
        return damage;
    }

    void SetPlayerDamage(int d) {
        damage = d < 0 ? 0 : d;
    }

    // Proficiency helpers (new)
    void IncreasePlayerProficiency(float amount = 0.05f)
    {
        proficiency += amount;
        if (proficiency > 1.0f) proficiency = 1.0f;
        if (proficiency < 0.0f) proficiency = 0.0f;
        std::cout << "Proficiency: " << proficiency << '\n';
    }

    void DecreasePlayerProficiency(float amount = 0.05f)
    {
        proficiency -= amount;
        if (proficiency < 0.0f) proficiency = 0.0f;
    }

    float GetPlayerProficiency() const {
        return proficiency;
    }

    // singleton access 
    static PlayerStats& Get()
    {
        static PlayerStats instance;
        return instance;
    }

    // reset (call at game start / new run)
    void Reset()
    {
        health = 5;
        maxHealth = 5;
        damage = 1;
        proficiency = 0.0f;
        speedMult = 1.0f;
        maxJumpStamina = BASE_MAX_STAMINA;
        jumpStamina = BASE_MAX_STAMINA;
    }

    void ResetCounters()
    {
        deathCount = 0;
        killCount = 0;
    }

private:
    // prevent external construction / copying
    PlayerStats() = default;
    PlayerStats(const PlayerStats&) = delete;
    PlayerStats& operator=(const PlayerStats&) = delete;
};

#endif // PLAYER_STATS_H