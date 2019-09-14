all: yeet yeemacs

yeet run:
	$(MAKE) -C yeet_lang/src


yeemacs:
	$(MAKE) -C yeemacs_editor


clean:
	rm -f yeet
	rm -f yeemacs
	$(MAKE) clean -C  yeemacs_editor
	$(MAKE) clean -C yeet_lang/src

