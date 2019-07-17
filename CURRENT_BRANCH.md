# Current Branch Worklist

- Remove old setup wizard.
- Remove main guide widget.
- Remove DGuidEd (from docs too).
- Convert 'Learn' buttons to open web pages in browser.
- Redo top bar
- Remove Setup Wizard - prefer instead to have complete wizards for adding the necessary partial sections.
- Add System Templates to startup screen, which provide skeletal system setups for common (or not-so-common) use cases, but without any details of processing (which is the job of the Add Layer Wizard - contents of that could be called from the Templates wizard). For example: pure liquid, liquid mixture, A solvated in water, A+B solvated in water, silica, A confined in MCM-41, water, A+B confined in MCM-41.  Other examples, such as "A absorbed in MOF", need to ask for a cif file for the MOF, for instance. Store as individual, complete input files somewhere, which are somehow stored (in Qt resource, if we only care about accessing them via the GUI?). Can test these files, loading them as input to dissolve-serial, to check for consistency.
