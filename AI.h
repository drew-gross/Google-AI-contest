#ifndef AI_H_
#define AI_H_

class AI {
public:
	static void DoTurn();
	static void FinishTurn();

private:
	static void DefensePhase();
	static void AttackPhase();
	static void SupplyPhase();
};

#endif //AI_H_
