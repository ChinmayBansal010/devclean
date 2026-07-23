_devclean() {
    local cur prev words cword
    _init_completion || return
    case "${prev}" in
        --report)
            COMPREPLY=( $(compgen -W "markdown html csv json" -- "$cur") )
            return 0
            ;;
        --category)
            COMPREPLY=( $(compgen -W "python rust javascript java build package managers containers editors" -- "$cur") )
            return 0
            ;;
    esac
    COMPREPLY=( $(compgen -W "scan analyze clean stats version --help --json --verbose --dry-run --force --category --exclude --sort --reverse --report" -- "$cur") )
}
complete -F _devclean devclean
