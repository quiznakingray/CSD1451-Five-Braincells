#include "EnemyCombat.h"
#include "PlayerGameObject.h"
#include "EnemyGameObject.h"
#include <iostream>
#include <cmath>

void EnemyAttackPlayer(EnemyBase& enemy, Player& player, AEVec2& enemyPos, float dt)
{
    if (!enemy.isAlive) return;

    if (enemy.attackTimer > 0.f)
        enemy.attackTimer -= dt;

    float dx = player.pos.x - enemyPos.x;
    float dy = player.pos.y - enemyPos.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance <= enemy.stats.attackRange) {
        if (enemy.attackTimer <= 0.f) {
            if (enemy.type == EnemyType::BASIC_RANGED ||
                enemy.type == EnemyType::MINI_BOSS_RANGED) {

                if (enemy.projectile) {
                    AEVec2 dir;
                    AEVec2Sub(&dir, &player.pos, &enemyPos);
                    AEVec2Normalize(&dir, &dir);

                    enemy.projectile->damage = enemy.stats.attack;
                    enemy.projectile->isEnemyProjectile = true;
                    enemy.projectile->ShootArrow(enemyPos, dir);

                    std::cout << "[EnemyCombat] Ranged enemy shot projectile\n";
                }
            }
            else {
                // Melee attack
                player.TakeDamage(enemy.stats.attack);
                std::cout << "[EnemyCombat] Enemy hit player for "
                    << enemy.stats.attack << " dmg\n";
            }

            enemy.attackTimer = enemy.stats.attackCooldown;

            // Set state to attack for animation / debug
            EnemyGameObject* go = dynamic_cast<EnemyGameObject*>(&enemy);
            if (go) go->currentState = EnemyState::ATTACK;
        }
    }
    else {
        std::cout << "[EnemyCombat] Player not in range. Distance: " << distance << "\n";
    }
}

bool IsEnemyDead(const EnemyBase& enemy)
{
    return enemy.stats.health <= 0;
}