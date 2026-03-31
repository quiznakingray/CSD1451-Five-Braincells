#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H
#include <iostream>
#include "AudioManager.h"
#include "GameStateManager.h"

enum class PLAYER_TYPE {
    MELEE,
    RANGE
};

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

    // timer
    float totalSeconds = 0;

    // current player
    PLAYER_TYPE playerType = PLAYER_TYPE::MELEE;

    // helpers
    // seconds betw arrow shots
    float GetAttackCooldown() const;

    float GetMaxShieldDuration() const;
    bool RegenStamina(double dt);

    bool ConsumeJumpStamina();

    void ReducePlayerHealth();

    void IncreasePlayerHealth(int amount = 1);

    int GetPlayerHealth() const;

    int GetPlayerMaxHealth() const;

    void SetPlayerHealth(int h);

    void SetPlayerMaxHealth(int mh);

    // Damage helpers (new)
    void IncreasePlayerDamage(int amount = 1);

    void DecreasePlayerDamage(int amount = 1);

    int GetPlayerDamage() const;

    void SetPlayerDamage(int d);

    // Proficiency helpers (new)
    void IncreasePlayerProficiency(float amount = 0.05f);

    void DecreasePlayerProficiency(float amount = 0.05f);

    float GetPlayerProficiency() const;

    void IncreaseDeathCounter();

    void SetTotalSeconds(float seconds);

    float GetTotalSeconds() const;

    void SetPlayerType(PLAYER_TYPE type);

    PLAYER_TYPE GetPlayerType();

    // singleton access 
    static PlayerStats& Get();

    // reset (call at game start / new run)
    void Reset();

    void ResetCounters();

private:
    // prevent external construction / copying
    PlayerStats() = default;
    PlayerStats(const PlayerStats&) = delete;
    PlayerStats& operator=(const PlayerStats&) = delete;
};

#endif // PLAYER_STATS_H