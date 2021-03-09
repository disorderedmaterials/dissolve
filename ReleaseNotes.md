Version 0.7.4 is a patch release addressing the following issues:

- Permit saving of calculated data to user-specified filename and format.

Input files currently using the `Save` keyword in modules such as `CalculateDAngle`, `CalculateRDF` etc. will need to be edited by hand to read, for example:

```
Save  xy  'myfile.txt'
EndSave
```

The full release notes for 0.7.0 can be found at https://github.com/disorderedmaterials/dissolve/releases/tag/0.7.0.
