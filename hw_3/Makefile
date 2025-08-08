.PHONY: all clean algorithm gameManager simulator

all: algorithm gameManager simulator

algorithm:
	$(MAKE) -C algorithm

gameManager:
	$(MAKE) -C gameManager

simulator: algorithm
	$(MAKE) -C simulator

clean:
	$(MAKE) -C algorithm clean
	$(MAKE) -C gameManager clean
	$(MAKE) -C simulator clean
