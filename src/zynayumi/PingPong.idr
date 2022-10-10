module PingPong

-- Code to understand how to implement pingpong effect in Zynayumi
-- sequencer.

||| Given loop, end and the current step count since the note is on,
||| return the corresponding sequencer index.
f : Integer -> Integer -> Integer -> Integer
f loop end cnt = let len = end - loop - 1
                 in if cnt < loop
                    then cnt
                    else loop + abs (((cnt + len - loop) `mod` (2 * len)) - len)

-- Test f

test_f0 : f 2 5 0 === 0
test_f0 = Refl

test_f1 : f 2 5 1 === 1
test_f1 = Refl

test_f2 : f 2 5 2 === 2
test_f2 = Refl

test_f3 : f 2 5 3 === 3
test_f3 = Refl

test_f4 : f 2 5 4 === 4
test_f4 = Refl

test_f5 : f 2 5 5 === 3
test_f5 = Refl

test_f6 : f 2 5 6 === 2
test_f6 = Refl

test_f7 : f 2 5 7 === 3
test_f7 = Refl

test_f8 : f 2 5 8 === 4
test_f8 = Refl

test_f9 : f 2 5 9 === 3
test_f9 = Refl

test_f10 : f 2 5 10 === 2
test_f10 = Refl

test_f11 : f 2 5 11 === 3
test_f11 = Refl

test_f12 : f 2 5 12 === 4
test_f12 = Refl

test_f13 : f 2 5 13 === 3
test_f13 = Refl
