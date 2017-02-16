class Hello 
{
public:
	enum Status {S_READY = 0, S_RUN = 2, S_DEAD = 3};
	void setStatus(Status);
private:
	Status s;
};
