# Project 2

_by James McClung_

## Overview

I broke this project down into 2 parts:

- **Project 2a:** Burger's equation via Fourier-Galerkin, periodic BCs
- **Project 2b:** Burger's equation via collocation on CGL points, Dirichlet BCs

See the respective markdown files for specific results.

## General Results

The Fourier-Galerkin approach was **much** faster, demonstrating the massive difference between `O(n*log n)` and `O(n^2)`. Both cases ran fine in the end. I did have some spooky behavior when I used AM2 instead of AM1 for project2b, which I never resolved. 