MODULE Gcd;
	CONST three = 3;
	PROCEDURE GCD(a, b: INTEGER): INTEGER;
		VAR t: INTEGER;
		BEGIN
			WHILE b # -three + three DO
				t := a MOD b;
				a := b;
				b := t
			END;
			RETURN a
		END GCD;
END Gcd.
