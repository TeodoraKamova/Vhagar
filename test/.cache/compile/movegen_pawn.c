// from a personal project (chess engline). Mofified to use only ints. Will not compute any
// meaningful stuff

int WE_add8(int si) { return 65280 << (si & ~(int)7); }

int ANDNOT(int a, int b) { return ((~(a)) & (b)); }

int PawnWhite(int ownPieces, int enemyPieces, int enPassant, int si)
{
    int piecesCombined;
    int normalMoveForward;
    int canMoveLongForward;
    int longMovesForwardBit;
    int longMoveForward;
    int normalCaptures;
    int enPassantCaptures;
    int captures;

    piecesCombined = ownPieces | enemyPieces;

    normalMoveForward = (256 << (si));
    canMoveLongForward = (si < 16) && !(piecesCombined & normalMoveForward);
    longMovesForwardBit = (canMoveLongForward) ? normalMoveForward : 0;
    longMoveForward = longMovesForwardBit << 8;

    // Proposal for captures including en-passant captures:
    normalCaptures = (640 << (si)) & WE_add8(si);
    enPassantCaptures = normalCaptures & enPassant;

    captures = (normalCaptures & enemyPieces) | enPassantCaptures;

    return captures | ANDNOT(piecesCombined, normalMoveForward | longMoveForward);
}

// will return 1 on unexpected output, 0 otherwise
int main()
{
    if (PawnWhite(65280, 0, 0, 8) != 16842752)
        return 1;
    if (PawnWhite(65280, 0, 0, 9) != 33685504)
        return 2;
    if (PawnWhite(65280, 0, 0, 10) != 67371008)
        return 3;
    if (PawnWhite(65280, 0, 0, 11) != 134742016)
        return 4;
    if (PawnWhite(65280, 0, 0, 12) != 269484032)
        return 5;
    if (PawnWhite(65280, 0, 0, 13) != 538968064)
        return 6;
    if (PawnWhite(65280, 0, 0, 14) != 1077936128)
        return 7;
    if (PawnWhite(65280, 0, 0, 15) != -2139095040)
        return 8;
    if (PawnWhite(65280, 0, 0, 16) != 16777216)
        return 9;
    if (PawnWhite(65280, 0, 0, 17) != 33554432)
        return 10;
    if (PawnWhite(65280, 0, 0, 18) != 67108864)
        return 11;
    if (PawnWhite(65280, 0, 0, 19) != 134217728)
        return 12;
    if (PawnWhite(65280, 0, 0, 20) != 268435456)
        return 13;
    if (PawnWhite(65280, 0, 0, 21) != 536870912)
        return 14;
    if (PawnWhite(65280, 0, 0, 22) != 1073741824)
        return 15;

    if (PawnWhite(1310720, 939524096, 0, 18) != 201326592)
        return 16;

    if (PawnWhite(1310720, 939524096, 0, 20) != 671088640)
        return 17;

    if (PawnWhite(65280, 0, 0, 24) != 0)
        return 18;

    // all successfull
    return 0;
}

