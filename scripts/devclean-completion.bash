_devclean() {
    local cur prev words cword
    _init_completion || return
    case "${prev}" in
        --report)
            COMPREPLY=( $(compgen -W "markdown html csv json" -- "$cur") )
            return 0
            ;;
    esac
    COMPREPLY=( $(compgen -W "scan analyze clean stats version" -- "$cur") )
}
complete -F _devclean devclean
