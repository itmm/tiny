MODULE Gcd;
	(* some small tests *)
	PROCEDURE GCD(a, b: INTEGER): INTEGER;
		VAR x, y, t: INTEGER;
		BEGIN
			x := a; y := b;
			WHILE y # 0 DO
				t := x MOD y;
				x := y;
				y := t
			END;
		RETURN x
	END GCD;
	PROCEDURE Min(a, b: INTEGER): INTEGER;
		VAR r: INTEGER;
		BEGIN
			IF a < b THEN
				r := a
			ELSE
				r := b
			END;
		RETURN r
	END Min;
	PROCEDURE Sum(a, b: INTEGER): INTEGER;
		BEGIN
		RETURN a + b
	END Sum;
	PROCEDURE Sum2(a, b: REAL): REAL;
		BEGIN
		RETURN a + b
	END Sum2;
	PROCEDURE Mod(a, b: INTEGER): INTEGER;
		BEGIN
		RETURN a MOD b
	END Mod;
END Gcd.
