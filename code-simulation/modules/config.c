char* config(void) {

    /* String format (space-separated pairs):
    first pair is <source_ttn>-<source_p2p>, then
    <x>-<y> pairs, with x for father and y for child 
    */
    char* buffer = "10-11 "
                   "11-12 "
                   "12-13 "
                   "12-14";
    
    return buffer;
}
