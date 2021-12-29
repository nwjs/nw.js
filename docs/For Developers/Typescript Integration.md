# Typescript Integration {:.doctitle}
---
## A. Using community definitions for global `nw` object

NW.js does not provide offical typescript definitions however you can integrate the community definitions by
1. Installing the [community type definitions package](https://github.com/DefinitelyTyped/DefinitelyTyped/tree/master/types/nw.gui): `@types/nw.gui`.
2. Creating a file called *nw.d.ts* with the following content:
```ts
declare var nw: typeof import('nw.gui')
```
After this typescript should not complain anymore about the "`undefined`" global `nw` object, 
also be aware that the community definitions may not always match the latest NW.js API, 
however if you face a problems with the current type definitions you can opt the solution B.

## B. Disabling type checking for global `nw` object

Create a file called *nw.d.ts* with the following content 

```ts
declare var nw: any
```
