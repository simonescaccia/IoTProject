char* config(void) {

    /* String format (space-separated pairs):
    first pair is <source_ttn>-<source_p2p>, then
    <x>-<y> pairs, with x for father and y for child 
    */
    char* buffer = "1-17 "
                   "17-18 "
                   "18-20";
    
    return buffer;
}
