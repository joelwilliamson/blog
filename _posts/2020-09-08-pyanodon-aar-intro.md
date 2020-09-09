---
layout: post
title: Introduction to Pyanodon
categories: [factorio, pyanodon]
---
I recently started a game of [Factorio](https://factorio.com/) using [Pyanodon's mods](https://mods.factorio.com/user/pyanodon).
This is a set of mods intended to make the production chains in Factorio more complex and realistic.
For example, in vanilla Factorio Electronic circuits are made by
combining copper wires and iron plates in an assembler.
This requires a total of 2 buildings to go from plates to final product.
In Pyanodons, Simple circuits (the equivalent item) require Capacitors, Inductors, Resistors, Vacuum Tubes and PCBs.
Each of the electronic components requires at least a metal plate or wire as well as Ceramic or Glass that needs to be assembled from simpler ingredients.
The PCBs have their own production chain, going from wood -> treated wood -> fiberboard -> formica -> PCB, pulling in at least one other ingredient at each step.
By my count, 27 different recipes are used, not including smelting ore into plates.

Someone on Reddit commented that there are lots of posts showing Simple Circuits, but it seems most people give up shortly after.
I thought I would chronicle my progress so people interested in the mod can see what's involved.

The aforementioned simple circuit assembly:
![Simple Circuit Assembly](/images/simple-circuit-assembly.jpg)

Raw coal goes through a two stage coking process to reduce it to coke, as well as creating some tar and coal gas.
![Coking](/images/small-coking.jpg)

Quartz crystals are refined into molten glass before being formed into glass plates or glass beakers for science.
![Glass](/images/small-glass.jpg)

Alien lifeforms are grown to provide urea and methane to create melamine.
![Melamine](/images/single-melamine.jpg)

Most metals can be smelted in a stone furnace like in vanilla, but this is very expensive, typically using around 10 ore/plate. Here I have a three-stage copper smelter that screens and crushes the ore before casting to improve yield.
![Three Stage Copper](/images/three-stage-copper.jpg)

I'll post more as the factory continues to grow.
