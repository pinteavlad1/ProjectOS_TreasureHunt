.PHONY: all treasure_hub treasure_manager score_calculator clean

all: treasure_hub treasure_manager score_calculator

treasure_hub:
	$(MAKE) -C treasure_hub

treasure_manager:
	$(MAKE) -C treasure_manager

score_calculator:
	gcc score_calculator/main.c -o bin/score_calculator

clean:
	$(MAKE) -C treasure_hub clean
	$(MAKE) -C treasure_manager clean
	rm -f bin/score_calculator