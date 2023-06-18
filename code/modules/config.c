char* config(void) {

    /* String format (space-separated pairs):
    first pair is <source_ttn>-<source_p2p>, then
    <x>-<y> pairs, with x for father and y for child 
    */
    char* buffer = "1-10 "
                   "10-11 "
                   "11-18 "
                   "11-19";
    
    return buffer;
}
