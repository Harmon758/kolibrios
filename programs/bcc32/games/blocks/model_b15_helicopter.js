// block name, color, step, coord: x,y,z, rotation: x,y,z

const m1 = 0xff0000; //red
const m2 = 0xffffff; //white
const m3 = 0x808080; //gray
const m4 = 0xffff00; //yellow
const m5 = 0x080808; //black

model_list=[
['b2x3x1', m1,  0, -4,-24, 3, 0,0,-90],
['b2x2x1', m2,  0, -8,-20, 3, 0,0,0],
['b2x2x1', m2,  0,  4,-20, 3, 0,0,0],
['b2x4x1', m1,  0, -8,-12, 3, 0,0,0],
['b2x4x1', m1,  0,  4,-12, 3, 0,0,0],
['b1x2x1', m1,  0, -8,  4, 3, 0,0,90],
['b1x2x1', m1,  0, 12,  4, 3, 0,0,90],
['b1x3x1', m1,  0, -8,  8, 3, 0,0,0],
['b1x3x1', m1,  0,  8,  8, 3, 0,0,0],
['b1x3x1', m3,  0,-12,  8, 3, 0,0,0],
['b1x3x1', m3,  0, 12,  8, 3, 0,0,0],
['b2x4x1', m1,  1,-12,  4, 6, 0,0,0],
['b2x4x1', m1,  1,  8,  4, 6, 0,0,0],
['b1x4x1', m1,  1,  0,-36, 6, 0,0,0],
['b1x4x1', m1,  1, -4,-32, 6, 0,0,0],
['b1x4x1', m1,  1,  4,-32, 6, 0,0,0],
['b1x4x1', m1,  1, -8,-24, 6, 0,0,0],
['b1x4x1', m1,  1,  8,-24, 6, 0,0,0],
['b2x3x1', m1,  1, -8, -8, 6, 0,0,0],
['b2x3x1', m1,  1,  4, -8, 6, 0,0,0],
['b1x1x1', m4,  1, -8,-28, 6, 0,0,0],
['b1x1x1', m4,  1,  8,-28, 6, 0,0,0],
['b2x3x1', m1,  2,  4,-36, 9, 0,0,90],
['b2x3x1', m1,  2, -8,-28, 9, 0,0,0],
['b2x3x1', m1,  2,  4,-28, 9, 0,0,0],
['b1x4x1', m2,  2, -8,-16, 9, 0,0,0],
['b1x4x1', m2,  2,  8,-16, 9, 0,0,0],
['b1x4x1', m2,  2, -8,  0, 9, 0,0,0],
['b1x4x1', m2,  2,  8,  0, 9, 0,0,0],
['b1x2x1', m1,  2, -8, 16, 9, 0,0,0],
['b1x2x1', m1,  2,  8, 16, 9, 0,0,0],
['b2x3x1', m1,  3,  4,  4, 9, 0,0,90],
['b1x3x1', m3,  3,  4,  4, 6, 0,0,90],
['b1x3x1', m3,  3,  4,  8, 6, 0,0,90],
['sf2x2x1', m1,  3,  0, 14, 4.5, 0,0,180],
['b1x4x1', m2,  4, -8, 12, 12, 0,0,0],
['b1x4x1', m2,  4,  8, 12, 12, 0,0,0],
['b2x8x1', m1,  4, -8,-20, 12, 0,0,0],
['b2x8x1', m1,  4,  4,-20, 12, 0,0,0],
['l2x2x1', m5,  4, -8,-24, 12, 0,0,-90],
['l2x2x1', m5,  4,  8,-24, 12, 0,0,-180],
['b2x3x1', m5,  4,  4,-32, 12, 0,0,90],
['l2x2x1', m5,  5, -8,-20, 15, 0,0,-90],
['l2x2x1', m5,  5,  8,-20, 15, 0,0,-180],
['b2x3x1', m5,  5,  4,-28, 15, 0,0,90],
['b1x1x1', m1,  5, -8,-16, 15, 0,0,0],
['b1x1x1', m1,  5,  8,-16, 15, 0,0,0],
['b1x4x1', m5,  5, -8,-12, 15, 0,0,0],
['b1x4x1', m5,  5,  8,-12, 15, 0,0,0],
['b1x4x1', m1,  5, -8,  4, 15, 0,0,0],
['b1x4x1', m1,  5,  8,  4, 15, 0,0,0],
['b2x2x1', m1,  6, -8,  8, 18, 0,0,0],
['b2x2x1', m1,  6,  4,  8, 18, 0,0,0],
['b1x4x1', m5,  6, -8, -8, 18, 0,0,0],
['b1x4x1', m5,  6,  8, -8, 18, 0,0,0],
['b1x1x1', m1,  6, -8,-12, 18, 0,0,0],
['b1x1x1', m1,  6,  8,-12, 18, 0,0,0],
['b2x3x1', m5,  6,  4,-24, 18, 0,0,90],
['l2x2x1', m5,  6, -8,-16, 18, 0,0,-90],
['l2x2x1', m5,  6,  8,-16, 18, 0,0,180],
['b1x3x1', m1,  7,  4,-20, 21, 0,0,90],
['b2x3x1', m1,  7, -8,-16, 21, 0,0,0],
['b2x3x1', m1,  7,  4,-16, 21, 0,0,0],
['b2x3x1', m1,  7, -8, -4, 21, 0,0,0],
['b2x3x1', m1,  7,  4, -4, 21, 0,0,0],
['b1x2x1', m1,  7, -8,  8, 21, 0,0,0],
['b2x4x1', m1,  7, -4, 12, 21, 0,0,-90],
['b1x3x1', m1,  7, -4, 16, 21, 0,0,-90],
['b2x3x1', m1,  8,  4,  0, 24, 0,0,90],
['b2x3x1', m1,  8,  4, -8, 24, 0,0,90],
['b2x3x1', m1,  8,  4,-16, 24, 0,0,90],
['f1x2x1', m3,  9,  0, -4, 27, 0,0,0],
['sm2x2x1', m1, 10,  0, 14, 4.5, 90,90,0],
['b2x8x1', m1, 11,  2, 14, -4.5, 0,0,-90, 1],
['b2x4x1', m1, 11, 42, 10, -4.5, 0,0,0, 1],
['b2x8x1', m1, 12, 46, 10, -1.5, 0,0,90, 1],
['b1x4x1', m1, 12, 46, 18, -1.5, 0,0,0, 1],
['b2x3x1', m1, 12, 14, 10, -1.5, 0,0,90, 1],
['f1x2x1', m3, 12, 42, 18, -1.5, 0,0,0, 1],
['b1x1x1', m3, 13, 42, 20, 1.5, 0,0,45, 1],
['b1x3x1', m3, 14,  0, -4, 3, 0,0,0, 2],
['b1x3x1', m3, 14,  4,  0, 6, 0,0,90, 2],
['b1x2x1', m1, 13, 46, 10, 1.5, 0,0,90, 1],
['b2x8x1', m1, 13,  2, 14, 1.5, 0,0,-90, 1],
['b1x2x1', m1, 15, 46, 10, 4.5, 0,0,90, 1],
['b1x1x1', m1, 15, 46, 10, 7.5, 0,0,90, 1],
['b1x2x1', m1, 15, 46, 10, -7.5, 0,0,90, 1],
['b1x2x1', m5, 16,  0,-28, 0, 0,0,0],
['b1x2x1', m5, 16,  8,  8, 0, 0,0,0],
['b1x2x1', m5, 16, -8,  8, 0, 0,0,0],
['b1x1x1', m1, 18,  0, -2, 36, 0,0,-45],
['b1x3x1', m1, 16,  0, -4, -6, 0,0,0, 1],
['b1x1x1', m1, 17,  0,  4, -3, 0,0,0, 1],
['b1x1x1', m1, 17,  0, -4, -3, 0,0,0, 1],
['b1x3x1', m1, 17,  4,  0, -3, 0,0,90, 1],
['b1x4x1', m3, 18,  0,  4, 0, 0,0,0, 1],
['b1x4x1', m3, 18, -4,  0, 0, 0,0,90, 1],
['b1x4x1', m3, 18,  0, -4, 0, 0,0,180, 1],
['b1x4x1', m3, 18,  4,  0, 0, 0,0,-90, 1],
['b1x3x1', m3, 19,  0, -4, 3, 0,0,0, 1],
['b1x1x1', m3, 19, -4,  0, 3, 0,0,0, 1],
['b1x1x1', m3, 19,  4,  0, 3, 0,0,0, 1],
['b2x8x1', m5, 19, -4, 12, 3, 0,0,0, 1],
['b2x8x1', m5, 19,-12, -4, 3, 0,0,90, 1],
['b2x8x1', m5, 19,  4,-12, 3, 0,0,180, 1],
['b2x8x1', m5, 19, 40,  0, 3, 0,0,90, 1],
['b1x3x1', m4, 20,  4,  0, 6, 0,0,90, 1],
['b1x1x1', m4, 20,  0, -4, 6, 0,0,0, 1],
['b1x1x1', m4, 20,  0,  4, 6, 0,0,0, 1],

];

model_animat=[
[90,'rotation','z','Screw 1', 0,3.141592654,.09817477],
[79,'rotation','z','Screw 2', 0,3.141592654,.09817477],
];
