// block name, color, step, coord: x,y,z, rotation: x,y,z

const m1 = 0x008000;
const m2 = 0xffffff;
const m3 = 0x080808;
const m4 = 0x00ff00;

model_list=[
['b10x10x1', m1, 0,-20, -20, 0, 0,0,0],
['b2x3x1', m2,  1, -4, -4, 3, 0,0,90],
['b2x3x1', m2,  1,  8, -4, 3, 0,0,90],
['b2x4x1', m2,  1,  4, 12, 3, 0,0,90],
['b2x4x1', m3,  1,-20, -12, 3, 0,0,0],
['b2x4x1', m3,  1, 12, -12, 3, 0,0,0],
['b2x8x1', m3,  1, 12, 4, 3, 0,0,90],
['b2x4x1', m3,  2,-20, -12, 6, 0,0,0],
['b2x4x1', m3,  2, 12, -12, 6, 0,0,0],
['b2x4x1', m2,  2,  4, -8, 6, 0,0,90],
['b2x3x1', m2,  2,-12, 8, 6, 0,0,0],
['b2x4x1', m2,  2, -4, 8, 6, 0,0,0],
['b2x3x1', m2,  2,  4, 8, 6, 0,0,0],
['b1x2x1', m3,  2, 12, 4, 6, 0,0,0],
['b1x2x1', m3,  2,-16, 4, 6, 0,0,0],
['l2x2x1', m2,  2,-12, 0, 6, 0,0,-90],
['l2x2x1', m2,  2,  8, 0, 6, 0,0,180],
['b2x2x1', m2,  3,  4, -8, 9, 0,0,0],
['b2x4x1', m2,  3, 0, -8, 9, 0,0,90],
['b2x4x1', m2,  3,  8, 0, 9, 0,0,0],
['b2x4x1', m2,  3,-16, 0, 9, 0,0,0],
['b1x3x1', m2,  3, -4, 16, 9, 0,0,90],
['b1x3x1', m2,  3, 0, 16, 9, 0,0,-90],
['b1x2x1', m2,  3, 0, 20, 9, 0,0,90],
['l2x2x1', m2,  4,  8, 12, 12, 0,0,0],
['l2x2x1', m2,  4,-12, 12, 12, 0,0,90],
['b2x4x1', m2,  4,  4, 12, 12, 0,0,90],
['b2x4x1', m2,  4,  4, -8, 12, 0,0,90],
['b2x2x1', m2,  4,-16, 4, 12, 0,0,0],
['b2x2x1', m2,  4,  8, 4, 12, 0,0,0],
['b2x3x1', m3,  4,-16, -8, 12, 0,0,0],
['b2x3x1', m3,  4,  8, -8, 12, 0,0,0],
['l2x2x1', m2,  5,  8, 8, 15, 0,0,0],
['l2x2x1', m2,  5,-12, 8, 15, 0,0,90],
['b2x4x1', m2,  5,  4, 12, 15, 0,0,90],
['b2x4x1', m2,  5,  4, -4, 15, 0,0,90],
['b2x4x1', m3,  5,-16, -8, 15, 0,0,0],
['b2x4x1', m3,  5,  8, -8, 15, 0,0,0],
['b2x4x1', m2,  6,  4, 12, 18, 0,0,90],
['b2x4x1', m2,  6,  4, -4, 18, 0,0,90],
['b1x3x1', m3,  6,-12, 4, 18, 0,0,0],
['b1x3x1', m3,  6,  8, 4, 18, 0,0,0],
['b2x4x1', m3,  7,  4, 8, 21, 0,0,90],
['b2x4x1', m2,  7,  4, 0, 21, 0,0,90],
['b1x2x1', m3,  7,-12, 8, 21, 0,0,0],
['b1x2x1', m3,  7,  8, 8, 21, 0,0,0],
['b2x3x1', m2,  8, -4, -8, 24, 0,0,0],
['b2x2x1', m2,  8,  4, -4, 24, 0,0,0],
['b2x2x1', m2,  8,-12, -4, 24, 0,0,0],
['b2x4x1', m2,  8, -4, 4, 24, 0,0,90],
['b2x4x1', m2,  8, 12, 4, 24, 0,0,90],
['b2x4x1', m2,  8,  4, 12, 24, 0,0,90],
['b2x4x1', m2,  9,  4, -8, 27, 0,0,90],
['b2x3x1', m2,  9,-16, 4, 27, 0,0,0],
['b2x3x1', m2,  9,  8, 4, 27, 0,0,0],
['b1x3x1', m2,  9, -4, 16, 27, 0,0,90],
['b1x3x1', m2,  9,  8, 16, 27, 0,0,90],
['l2x2x1', m2,  9,-12, 0, 27, 0,0,180],
['l2x2x1', m2,  9,  8, 0, 27, 0,0,-90],
['b1x2x1', m3, 10, 0, -8, 30, 0,0,90],
['b1x2x1', m3, 10,-12, -4, 30, 0,0,0],
['b1x2x1', m3, 10,  8, -4, 30, 0,0,0],
['b1x4x1', m2, 10,-16, 0, 30, 0,0,0],
['b1x4x1', m2, 10, 12, 0, 30, 0,0,0],
['b2x4x1', m2, 10,  4, -4, 30, 0,0,90],
['b2x3x1', m2, 10, -4, 12, 30, 0,0,90],
['b2x3x1', m2, 10,  8, 12, 30, 0,0,90],
['b1x2x1', m3, 11, -8, -4, 33, 0,0,90],
['b1x2x1', m3, 11,  8, -4, 33, 0,0,90],
['b2x4x1', m2, 11,-16, 0, 33, 0,0,0],
['b2x4x1', m2, 11,  8, 0, 33, 0,0,0],
['b2x3x1', m2, 11, -4, -4, 33, 0,0,0],
['b2x4x1', m2, 11,  4, 12, 33, 0,0,90],
['b2x4x1', m2, 12,-12, 0, 36, 0,0,0],
['b2x4x1', m2, 12, -4, 0, 36, 0,0,0],
['b2x4x1', m2, 12,  4, 0, 36, 0,0,0],
['b2x4x1', m2, 13,  4, 4, 39, 0,0,90],
['b1x2x1', m3, 13,-12, 12, 39, 0,0,90],
['b1x2x1', m3, 13,  8, 12, 39, 0,0,-90],
['b1x2x1', m3, 14,-12, 12, 42, 0,0,90],
['b1x2x1', m3, 14,  8, 12, 42, 0,0,-90],
['c1x1x1', m4, 15,-12, -16, 3, 0,0,0],
['c1x1x1', m4, 15,-12, -16, 6, 0,0,0],
['c1x1x1', m4, 15,-12, -16, 9, 0,0,0],
['c1x1x1', m4, 15,-12, -16, 12, 0,0,0],
['c1x1x1', m4, 15,-12, -16, 15, 0,0,0],
['b1x2x1', m4, 15,-12, -16, 18, 0,0,135],
['c1x1x1', m4, 15,-12, -16, 21, 0,0,0],
['c1x1x1', m4, 16, -8, -20, 3, 0,0,0],
['c1x1x1', m4, 16, -8, -20, 6, 0,0,0],
['c1x1x1', m4, 16, -8, -20, 9, 0,0,0],
['c1x1x1', m4, 16, -8, -20, 12, 0,0,0],
['b1x2x1', m4, 16, -8, -20, 15, 0,0,-135],
['c1x1x1', m4, 16, -8, -20, 18, 0,0,0],
];