#include <tiny_ecs.hpp>
#include "ai_decisionTree.hpp"
#include "common.hpp"
#include <components.hpp>
#include "tiny_ecs_registry.hpp"
#include <math_helpers.hpp>
#include <iostream>

enum class BTState {
	Running,
	Success,
	Failure
};

class BTNode {
public:
	virtual void init(Entity e) {};

	virtual BTState process(Entity e) = 0;
};

class BTRunPair : public BTNode {
private:
	int m_index;
	BTNode* m_children[2];

public:
	BTRunPair(BTNode* c0, BTNode* c1)
		: m_index(0) {
		m_children[0] = c0;
		m_children[1] = c1;
	}

	void init(Entity e) override
	{
		m_index = 0;
		// initialize the first child
		const auto& child = m_children[m_index];
		child->init(e);
	}

	BTState process(Entity e) override {
		if (m_index >= 2)
			return BTState::Success;

		// process current child
		BTState state = m_children[m_index]->process(e);

		// select a new active child and initialize its internal state
		if (state == BTState::Success) {
			++m_index;
			if (m_index >= 2) {
				return BTState::Success;
			}
			else {
				m_children[m_index]->init(e);
				return BTState::Running;
			}
		}
		else {
			return state;
		}
	}
};

class BTIfCondition : public BTNode
{
public:
	BTIfCondition(BTNode* c1, BTNode* c2, std::function<bool(Entity)> condition)
		: m_child_1(c1), m_child_2(c2), m_condition(condition) {
	}

	virtual void init(Entity e) override {
		m_child_1->init(e);
		m_child_2->init(e);
	}

	virtual BTState process(Entity e) override {
		if (m_condition(e))
			return m_child_1->process(e);
		else
			return m_child_2->process(e);
	}

private:
	BTNode* m_child_1;
	BTNode* m_child_2;
	std::function<bool(Entity)> m_condition;
};

class IdleTrack : public BTNode {
public:
	IdleTrack(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		std::cout << "IdleTrack";
		BossStatus& boss_status = registry.bossStatus.get(e);
		boss_status.boss_state = BOSS_STATE::IDLE_TRACK;
		boss_status.state_switch_timer = boss_status.idle_track_time;

		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};

class IdleWander : public BTNode {
public:
	IdleWander(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		std::cout << "IdleWander";
		BossStatus& boss_status = registry.bossStatus.get(e);
		boss_status.boss_state = BOSS_STATE::IDLE_WANDER;
		boss_status.state_switch_timer = boss_status.idle_wander_time;

		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};

class Evasion : public BTNode {
public:
	Evasion(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		std::cout << "Evasion";
		BossStatus& boss_status = registry.bossStatus.get(e);
		boss_status.boss_state = BOSS_STATE::EVADE;
		boss_status.state_switch_timer = boss_status.evasion_time;

		boss_status.movement_direction = normalize(boss_status.movement_direction);


		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};

class ChargeAttack : public BTNode {
public:
	ChargeAttack(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		std::cout << "ChargeAttack";
		BossStatus& boss_status = registry.bossStatus.get(e);
		Motion& boss_motion = registry.motions.get(e);
		boss_status.boss_state = BOSS_STATE::CHARGE_ATTACK;
		boss_status.state_switch_timer = boss_status.charge_attack_time;
		boss_status.movement_direction = move_toward(boss_status.attack_target_pos, boss_motion.position);
		
		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};

class RangeAttack : public BTNode {
public:
	RangeAttack(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		std::cout << "RangeAttack";
		BossStatus& boss_status = registry.bossStatus.get(e);
		boss_status.boss_state = BOSS_STATE::RANGE_ATTACK;
		boss_status.state_switch_timer = boss_status.range_attack_time;

		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};



void boss_bee_decision(Entity boss_entity) {
	BossStatus& boss_status = registry.bossStatus.get(boss_entity);

	IdleTrack idle_track(1);
	IdleWander idle_wander(1);
	Evasion evasion(1);
	ChargeAttack charge_attack(1);
	RangeAttack range_attack(1);

	std::function<bool(Entity)> player_in_attack_range = [](Entity e) {
		BossStatus& boss_status = registry.bossStatus.get(e);
		Entity player = registry.players.entities[0];
		Motion& boss_motion = registry.motions.get(e);
		Motion& player_motion = registry.motions.get(player);
		float player_dist = pos_distance(boss_motion.position, player_motion.position);
		bool in_range = false;
		if (boss_status.boss_mode == BOSS_MODE::CHARGE_MODE) {
			if (boss_status.charge_attack_cd_timer < 0) {
				in_range = boss_status.charge_range > player_dist;
				boss_status.charge_attack_cd_timer = boss_status.charge_attack_cd_time;
			}
			
		}
		else {
			in_range = boss_status.range_range > player_dist;
		}
		if (in_range) {
			boss_status.attack_target_entity = player;
			boss_status.attack_target_pos = player_motion.position;
		}
		return in_range;
		 };

	BTIfCondition charge_mode_action(&charge_attack, &idle_track, player_in_attack_range);
	BTIfCondition range_mode_action(&range_attack, &idle_wander, player_in_attack_range);

	std::function<bool(Entity)> if_evasion = [](Entity e) {
		BossStatus& boss_status = registry.bossStatus.get(e);
		
		Motion& boss_motion = registry.motions.get(e);
		bool evasion = false;
		for (Entity e : registry.webshoots.entities) {
			Motion& webshoot_motion = registry.motions.get(e);
			WebShoot& webshoot = registry.webshoots.get(e);
			float web_dist = pos_distance(boss_motion.position, webshoot.target_pos);
			if (boss_status.evasion_range > web_dist && boss_status.evasion_cd_timer < 0) {
				evasion = true;
				boss_status.evasion_cd_timer = boss_status.evasion_cd_time;
				if (webshoot_motion.position.x < boss_motion.position.x) {
					boss_status.movement_direction = vec2((webshoot_motion.velocity.y - boss_motion.position.y), (boss_status.default_pos.y - boss_motion.position.y));
				}
				else {
					boss_status.movement_direction = vec2(-(webshoot_motion.velocity.y - boss_motion.position.y), (boss_status.default_pos.y - boss_motion.position.y));
				}
				
				
			}
				
		}
		return evasion;
		};

	BTIfCondition charge_evasion_action(&evasion, &charge_mode_action, if_evasion);
	BTIfCondition range_evasion_action(&evasion, &range_mode_action, if_evasion);

	//Determine boss mode
	std::function<bool(Entity)> boss_mode = [](Entity e) {
		BossStatus& boss_status = registry.bossStatus.get(e);
		return boss_status.boss_mode == BOSS_MODE::CHARGE_MODE; };

	BTIfCondition boss_mode_action(&charge_evasion_action, &range_evasion_action, boss_mode);


	//Determine boss state
	std::function<bool(Entity)> boss_state = [](Entity e) {
		BossStatus& boss_status = registry.bossStatus.get(e);
		return (boss_status.boss_state == BOSS_STATE::FREEZE || boss_status.boss_state == BOSS_STATE::RETURN); };




	
	boss_mode_action.init(boss_entity);
	BTState state = boss_mode_action.process(boss_entity);

}