#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

struct PlayerStats
{

    // base constants
    static constexpr float BASE_COOLDOWN = 0.5f;
    static constexpr float MIN_COOLDOWN = 0.05f;
    static constexpr float BASE_SHIELD_DURATION = 2.0f;
    static constexpr float SHIELD_DURATION_SCALE = 5.0f; // extra seconds added at max proficiency

    static constexpr int   BASE_MAX_STAMINA = 3;     // starting cap
    static constexpr int   HARD_MAX_STAMINA = 7;     // powerup ceiling
    static constexpr float STAMINA_REGEN_RATE = 1.0f;  // charges per second

    // upgradeable stats
    int health = 5;       // current hearts
    int maxHealth = 5;       // maximum hearts
    int damage = 1; // damage dealt per hit
    float proficiency = 0.0f;    // higher = faster cooldowns for bow/longer duration for shield (value between 0 - 1)
    float speedMult = 1.0f;    // movement speed multiplier (1.0 = base)

    // jumping stamina
    int   maxJumpStamina = BASE_MAX_STAMINA;
    float jumpStamina = static_cast<float>(BASE_MAX_STAMINA);

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
    bool RegenStamina(float dt)
    {
        float cap = static_cast<float>(maxJumpStamina);
        if (jumpStamina >= cap) return false;

        jumpStamina += STAMINA_REGEN_RATE * dt;
        if (jumpStamina > cap)
            jumpStamina = cap;
        return true;
    }

    bool ConsumeJumpStamina()
    {
        if (jumpStamina < 1.0f) return false;
        jumpStamina -= 1.0f;
        return true;
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
        jumpStamina = static_cast<float>(BASE_MAX_STAMINA);
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