# Firmware for Satlink WS-6933

The project is practically finished, I'm wondering if I publish the code...

## Combination receivers WS-6933 (channel demodulator + tuner)

| N  | Channel demodulator | Tuner    | Single chip | DVB-S | DVB-S2 | | Tested |
|:---|:-------------------:|:--------:|:-----------:|:-----:|:------:|-|:------:|
| 1  | -                   | -        | M88RS2000   | x     | -      | | -      |
| 2  | -                   | -        | RS6000B     |       | x      | | +      |
| 3  | DS3002B             | TS2022   | -           | x     | x      | | -      |
| 4  |                     | RDA5815M | -           | x     | x      | | -      |
| 5  | DS3103              | TS2022   | -           | x     | x      | | -      |
| 6  |                     | RDA5815M | -           | x     | x      | | -      |
| 7  | DS3103B             | TS2022   | -           | x     | x      | | -      |
| 8  |                     | RDA5815M | -           | x     | x      | | +      |
| 9  | DS3103C             | TS2022   | -           | x     | x      | | -      |
| 10 |                     | RDA5815M | -           | x     | x      | | -      |

| Series | N     |
|:------:|:------|
| A      |       |
| L      | 2, 10 |
| S      |       |
| T      | 8     |
