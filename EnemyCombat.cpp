#include "EnemyCombat.h"
#include "PlayerGameObject.h"
#include "PlayerStats.h"
#include <iostream>
#include <cmath>

void EnemyAttackPlayer(EnemyBase& enemy, Player& player, AEVec2& enemyPos, float dt)
{
    if (!enemy.isAlive) return;

    // Update attack cooldown timer
    if (enemy.timeSinceLastAttack > 0.f)
        enemy.timeSinceLastAttack -= dt;

    // Compute squared distance to avoid sqrt
    float dx = player.pos.x - enemyPos.x;
    float dy = player.pos.y - enemyPos.y;
    float distanceSq = dx * dx + dy * dy;
    float attackRangeSq = enemy.stats.attackRange * enemy.stats.attackRange;

    // Debug info
    //std::cout << "[EnemyCombat] dist^2=" << distanceSq
    //    << "  range^2=" << attackRangeSq
    //    << "  timer=" << enemy.timeSinceLastAttack << "\n";

    // Out of range or still on cooldown
    if (distanceSq > attackRangeSq)
    {
        if (enemy.canMove) enemy.currentState = EnemyState::PATROL; // Return to patrol
        return;
    }
    if (enemy.timeSinceLastAttack > 0.f) return;

    // Determine attack type
    bool isRangedType = (enemy.type == EnemyType::BASIC_RANGED ||
        enemy.type == EnemyType::MINI_BOSS_RANGED);

    enemy.currentState = EnemyState::ATTACK;

    if (isRangedType)
    {
        if (enemy.projectile && distanceSq > 0.0001f)
        {
            float distance = sqrtf(distanceSq);
            AEVec2 dir = { dx / distance, dy / distance };
            AEVec2Normalize(&dir, &dir);

            enemy.projectile->damage = enemy.stats.damage;
            enemy.projectile->isEnemyProjectile = true;
            enemy.projectile->isActive = true;
            enemy.projectile->ShootArrow(enemyPos, dir);

            std::cout << "[EnemyCombat]["
                << (enemy.type == EnemyType::BASIC_RANGED ? "BASIC_RANGED" : "MINI_BOSS_RANGED")
                << "] Fired arrow dir=(" << dir.x << "," << dir.y
                << ") dmg=" << enemy.stats.damage << "\n";
        }
    }
    else
    {
        player.TakeDamage(enemy.stats.damage);

        std::cout << "[EnemyCombat]["
            << (enemy.type == EnemyType::BASIC_MELEE ? "BASIC_MELEE" : "MINI_BOSS_MELEE")
            << "] Hit player for " << enemy.stats.damage << " dmg\n";
    }

    enemy.timeSinceLastAttack = enemy.stats.attackCooldown;
}

bool IsEnemyDead(EnemyBase& enemy)
{
    if (enemy.stats.health <= 0)
    {
        enemy.isAlive = false;
        enemy.currentState = EnemyState::IDLE;
        return true;
    }
    return false;
}

void EnemyTakeDamage(EnemyGameObject& enemy, int damage)
{
    if (!enemy.base.isAlive) return;
    enemy.base.stats.health -= damage;
    
    // show damage number
    Text* t = enemy.AddComponent(new Text());
    t->SetText("-" + std::to_string(damage));
    t->center = { (-0.5f + AERandFloat() ) * enemy.scale.x  , 0};
    enemy.hurtTexts.push_back({ t, 0.f });

    std::cout << "[EnemyCombat] Enemy took " << damage << " dmg, health now " << enemy.base.stats.health << "\n";
    if (IsEnemyDead(enemy.base))
    {
        if (enemy.base.type == EnemyType::BASIC_MELEE ||
            enemy.base.type == EnemyType::BASIC_RANGED)
        {
            AudioManager::GetInstance().PlaySFX("enemyDie");
        }
        else {
            AudioManager::GetInstance().PlaySFX("minibossDie");
        }
        std::cout << "[EnemyCombat] Enemy died!\n";
        PlayerStats::GetInstance().killCount++;
    }
}