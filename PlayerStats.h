#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H
#include "SaveManager.h"
#include "AudioManager.h"
#include "GameStateManager.h"

struct PlayerStats
{

    // base constants
    static constexpr float BASE_COOLDOWN = 0.5f;
    static constexpr float MIN_COOLDOWN = 0.05f;
    static constexpr float BASE_SHIELD_DURATION = 2.0f;
    static constexpr float SHIELD_DURATION_SCALE = 5.0f; // extra seconds added at max proficiency

    // upgradeable stats
    int health = 5;       // current hearts
    int maxHealth = 5;       // maximum hearts
    int damage = 1; // damage dealt per hit
    float proficiency = 0.0f;    // higher = faster cooldowns for bow/longer duration for shield (value between 0 - 1)
    float speedMult = 1.0f;    // movement speed multiplier (1.0 = base)

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

    void ReducePlayerHealth()
    {
        health--;
        if (health > 0) {
            AudioManager::PlaySFX("playerHurt");
        }
        else
        {
            AudioManager::PlaySFX("playerDie");
            health = maxHealth;
            SaveManager::GetInstance().toContinue = true;
            GAME_STATE_TYPE respawnLevel = SaveManager::GetInstance().mapSaveData.savedLevel;
            current = GAME_STATE_TYPE::MENU;  // force inner loop to exit
            next = respawnLevel;           // reload the saved level
        }
    }

    void IncreasePlayerHealth() {
        health++;
        std::cout << health << '\n';
    }

    int GetPlayerHealth() {
        return health;
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
    }

private:
    // prevent external construction / copying
    PlayerStats() = default;
    PlayerStats(const PlayerStats&) = delete;
    PlayerStats& operator=(const PlayerStats&) = delete;
};

#endif // PLAYER_STATS_H