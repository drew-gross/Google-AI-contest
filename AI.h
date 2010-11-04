#ifndef AI_H_
#define AI_H_

class AI {
public:
	void DoTurn();
	void FinishTurn();

private:
	void DefensePhase();
	void AttackPhase();
	void SupplyPhase();
};

#endif //AI_H_
