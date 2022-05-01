MODULE Gcd;
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
	PROCEDURE Mod(a, b: INTEGER): INTEGER;
		BEGIN
		RETURN a MOD b
	END Mod;
END Gcd.
