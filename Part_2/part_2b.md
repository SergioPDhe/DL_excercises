### part B - using channels 5 or 6

Using channels 5 or 6 (only on STM32F3xx on TIMER1 where they are available) is more challenging because they are not connected to any external pins.
By looking at the bloc diagram on the datasheet, they must be acessed directly from OC5 or OC6 to be used.

If using these channels were necessary, I would have to directly call the state of OC5/6 and effectively push its value to an external GPIO (maybe with  memory2memory DMA)