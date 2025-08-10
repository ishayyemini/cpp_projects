.PHONY: all clean algorithm gameManager simulator

all: algorithm gameManager simulator

algorithm:
	$(MAKE) -C Algorithm

gameManager:
	$(MAKE) -C GameManager

simulator: algorithm
	$(MAKE) -C Simulator

clean:
	$(MAKE) -C Algorithm clean
	$(MAKE) -C GameManager clean
	$(MAKE) -C Simulator clean
