## Release 0.7.0

### Incompatibilities with 0.6.X

- The `Weights` class is now known as `NeutronWeights` in order to make way for the new `XRayWeights` class. Existing restart files will still contain references to `Weights`, which can be search/replaced to `NeutronWeights` to allow compatibility with 0.7.X.
