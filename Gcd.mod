MODULE Gcd;
	PROCEDURE GCD(a, b: INTEGER): INTEGER;
		VAR t: INTEGER;
		BEGIN
			WHILE b # 0 DO
				t := a MOD b;
				a := b;
				b := t
			END;
			RETURN a
		END GCD;
END Gcd.
