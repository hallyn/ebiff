PLUGINS=$(shell ../../contrib/checkplugins.sh | grep "^PLUGINS" | cut -d ":" -f 2)

all:
	for X in $(PLUGINS); do make -C $$X all; done

clean:
	for X in $(PLUGINS); do make -C $$X clean; done

