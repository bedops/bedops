/* 
   convert2bed.c
   -----------------------------------------------------------------------
   Copyright (C) 2014-2020 Alex Reynolds
   
   wig2bed components, (C) 2011-2020 Scott Kuehn and Shane Neph

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "convert2bed.h"

int
main(int argc, char** argv)
{
#ifdef DEBUG
    fprintf (stderr, "--- convert2bed main() - enter ---\n");
#endif

    struct stat stats;
    int stats_res;
    c2b_pipeset_t pipes;

    /* setup */
    c2b_init_globals();
    c2b_init_command_line_options(argc, argv);
    /* check that stdin is available */
    if ((stats_res = fstat(STDIN_FILENO, &stats)) == -1) {
        int errsv = errno;
        fprintf(stderr, "Error: fstat() call failed (%s)", (errsv == EBADF ? "EBADF" : (errsv == EIO ? "EIO" : "EOVERFLOW")));
        c2b_print_usage(stderr);
        return errsv;
    }
    if ((S_ISCHR(stats.st_mode) == kTrue) && (S_ISREG(stats.st_mode) == kFalse)) {
        fprintf(stderr, "Error: No input is specified; please redirect or pipe in formatted data\n");
        c2b_print_usage(stderr);
        return ENODATA; /* No message is available on the STREAM head read queue (POSIX.1) */
    }
    c2b_test_dependencies();
    c2b_init_pipeset(&pipes, MAX_PIPES);

    /* convert */
    c2b_init_conversion(&pipes);

    /* clean-up */
    c2b_delete_pipeset(&pipes);
    c2b_delete_globals();

#ifdef DEBUG
    fprintf (stderr, "--- convert2bed main() - exit  ---\n");
#endif
    return EXIT_SUCCESS;
}

static void
c2b_init_conversion(c2b_pipeset_t* p)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_conversion() - enter ---\n");
#endif

    switch(c2b_globals.input_format_idx)
        {
        case BAM_FORMAT:
            c2b_init_bam_conversion(p);
            break;
        case GFF_FORMAT:
            c2b_init_gff_conversion(p);
            break;
        case GTF_FORMAT:
            c2b_init_gtf_conversion(p);
            break;
        case GVF_FORMAT:
            c2b_init_gvf_conversion(p);
            break;
        case PSL_FORMAT:
            c2b_init_psl_conversion(p);
            break;
        case RMSK_FORMAT:
            c2b_init_rmsk_conversion(p);
            break;
        case SAM_FORMAT:
            c2b_init_sam_conversion(p);
            break;
        case VCF_FORMAT:
            c2b_init_vcf_conversion(p);
            break;
        case WIG_FORMAT:
            c2b_init_wig_conversion(p);
            break;
        default:
            fprintf(stderr, "Error: Currently unsupported format\n");
            c2b_print_usage(stderr);
            exit(ENOTSUP); /* Operation not supported (POSIX.1) */
        }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_conversion() - exit  ---\n");
#endif
}

static void
c2b_init_gff_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, &c2b_line_convert_gff_to_bed_unsorted);
}

static void
c2b_init_gtf_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, &c2b_line_convert_gtf_to_bed_unsorted);
}

static void
c2b_init_gvf_conversion(c2b_pipeset_t* p)
{
    /* GVF format conversion uses the GFF functor */
    c2b_init_generic_conversion(p, &c2b_line_convert_gff_to_bed_unsorted);
}

static void
c2b_init_psl_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, &c2b_line_convert_psl_to_bed_unsorted);
}

static void
c2b_init_rmsk_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, &c2b_line_convert_rmsk_to_bed_unsorted);
}

static void
c2b_init_sam_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, (!c2b_globals.split_flag ?
                                    &c2b_line_convert_sam_to_bed_unsorted_without_split_operation :
                                    &c2b_line_convert_sam_to_bed_unsorted_with_split_operation));
}

static void
c2b_init_vcf_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, &c2b_line_convert_vcf_to_bed_unsorted);
}

static void
c2b_init_wig_conversion(c2b_pipeset_t* p)
{
    c2b_init_generic_conversion(p, &c2b_line_convert_wig_to_bed_unsorted);
}

static void
c2b_init_generic_conversion(c2b_pipeset_t* p, void(*to_bed_line_functor)(char **, ssize_t *, ssize_t *, char *, ssize_t))
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_generic_conversion() - enter ---\n");
#endif

    pthread_t cat2generic_thread; 
    pthread_t generic2bed_unsorted_thread; 
    pthread_t bed_unsorted2stdout_thread;
    pthread_t bed_unsorted2bed_sorted_thread;
    pthread_t bed_sorted2stdout_thread;
    pthread_t bed_sorted2starch_thread;
    pthread_t starch2stdout_thread;
    c2b_pipeline_stage_t cat2generic_stage;
    c2b_pipeline_stage_t generic2bed_unsorted_stage;
    c2b_pipeline_stage_t bed_unsorted2stdout_stage;
    c2b_pipeline_stage_t bed_unsorted2bed_sorted_stage;
    c2b_pipeline_stage_t bed_sorted2stdout_stage;
    c2b_pipeline_stage_t bed_sorted2starch_stage;
    c2b_pipeline_stage_t starch2stdout_stage;
    char cat2generic_cmd[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    char bed_unsorted2bed_sorted_cmd[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    char bed_sorted2starch_cmd[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    void (*generic2bed_unsorted_line_functor)(char **, ssize_t *, ssize_t *, char *, ssize_t) = to_bed_line_functor;
    ssize_t buffer_size = C2B_THREAD_IO_BUFFER_SIZE;
    int errsv = 0;

    if ((!c2b_globals.sort->is_enabled) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        cat2generic_stage.pipeset = p;
        cat2generic_stage.line_functor = NULL;
        cat2generic_stage.src = -1;
        cat2generic_stage.dest = 0;
        cat2generic_stage.description = "Generic data from stdin";
        cat2generic_stage.pid = 0;
        cat2generic_stage.status = 0;
        cat2generic_stage.buffer_size = buffer_size;
        
        generic2bed_unsorted_stage.pipeset = p;
        generic2bed_unsorted_stage.line_functor = generic2bed_unsorted_line_functor;
        generic2bed_unsorted_stage.src = 0;
        generic2bed_unsorted_stage.dest = 1;
        generic2bed_unsorted_stage.description = "Generic data to unsorted BED";
        generic2bed_unsorted_stage.pid = 0;
        generic2bed_unsorted_stage.status = 0;
        generic2bed_unsorted_stage.buffer_size = buffer_size;

        bed_unsorted2stdout_stage.pipeset = p;
        bed_unsorted2stdout_stage.line_functor = NULL;
        bed_unsorted2stdout_stage.src = 1;
        bed_unsorted2stdout_stage.dest = -1;
        bed_unsorted2stdout_stage.description = "Unsorted BED to stdout";
        bed_unsorted2stdout_stage.pid = 0;
        bed_unsorted2stdout_stage.status = 0;
        bed_unsorted2stdout_stage.buffer_size = buffer_size;
    }
    else if (c2b_globals.output_format_idx == BED_FORMAT) {
        cat2generic_stage.pipeset = p;
        cat2generic_stage.line_functor = NULL;
        cat2generic_stage.src = -1;
        cat2generic_stage.dest = 0;
        cat2generic_stage.description = "Generic data from stdin"; 
        cat2generic_stage.pid = 0;
        cat2generic_stage.status = 0;
        cat2generic_stage.buffer_size = buffer_size;
        
        generic2bed_unsorted_stage.pipeset = p;
        generic2bed_unsorted_stage.line_functor = generic2bed_unsorted_line_functor;
        generic2bed_unsorted_stage.src = 0;
        generic2bed_unsorted_stage.dest = 1;
        generic2bed_unsorted_stage.description = "Generic data to unsorted BED";
        generic2bed_unsorted_stage.pid = 0;
        generic2bed_unsorted_stage.status = 0;
        generic2bed_unsorted_stage.buffer_size = buffer_size;
        
        bed_unsorted2bed_sorted_stage.pipeset = p;
        bed_unsorted2bed_sorted_stage.line_functor = NULL;
        bed_unsorted2bed_sorted_stage.src = 1;
        bed_unsorted2bed_sorted_stage.dest = 2;
        bed_unsorted2bed_sorted_stage.description = "Unsorted BED to sorted BED";
        bed_unsorted2bed_sorted_stage.pid = 0;
        bed_unsorted2bed_sorted_stage.status = 0;
        bed_unsorted2bed_sorted_stage.buffer_size = buffer_size;

        bed_sorted2stdout_stage.pipeset = p;
        bed_sorted2stdout_stage.line_functor = NULL;
        bed_sorted2stdout_stage.src = 2;
        bed_sorted2stdout_stage.dest = -1;
        bed_sorted2stdout_stage.description = "Sorted BED to stdout";
        bed_sorted2stdout_stage.pid = 0;
        bed_sorted2stdout_stage.status = 0;
        bed_sorted2stdout_stage.buffer_size = buffer_size;
    }
    else if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        cat2generic_stage.pipeset = p;
        cat2generic_stage.line_functor = NULL;
        cat2generic_stage.src = -1;
        cat2generic_stage.dest = 0;
        cat2generic_stage.description = "Generic data from stdin";
        cat2generic_stage.pid = 0;
        cat2generic_stage.status = 0;
        cat2generic_stage.buffer_size = buffer_size;
        
        generic2bed_unsorted_stage.pipeset = p;
        generic2bed_unsorted_stage.line_functor = generic2bed_unsorted_line_functor;
        generic2bed_unsorted_stage.src = 0;
        generic2bed_unsorted_stage.dest = 1;
        generic2bed_unsorted_stage.description = "Generic data to unsorted BED";
        generic2bed_unsorted_stage.pid = 0;
        generic2bed_unsorted_stage.status = 0;
        generic2bed_unsorted_stage.buffer_size = buffer_size;

        bed_unsorted2bed_sorted_stage.pipeset = p;
        bed_unsorted2bed_sorted_stage.line_functor = NULL;
        bed_unsorted2bed_sorted_stage.src = 1;
        bed_unsorted2bed_sorted_stage.dest = 2;
        bed_unsorted2bed_sorted_stage.description = "Unsorted BED to sorted BED";
        bed_unsorted2bed_sorted_stage.pid = 0;
        bed_unsorted2bed_sorted_stage.status = 0;
        bed_unsorted2bed_sorted_stage.buffer_size = buffer_size;

        bed_sorted2starch_stage.pipeset = p;
        bed_sorted2starch_stage.line_functor = NULL;
        bed_sorted2starch_stage.src = 2;
        bed_sorted2starch_stage.dest = 3;
        bed_sorted2stdout_stage.description = "Sorted BED to Starch";
        bed_sorted2starch_stage.pid = 0;
        bed_sorted2starch_stage.status = 0;
        bed_sorted2starch_stage.buffer_size = buffer_size;

        starch2stdout_stage.pipeset = p;
        starch2stdout_stage.line_functor = NULL;
        starch2stdout_stage.src = 3;
        starch2stdout_stage.dest = -1;
        starch2stdout_stage.description = "Starch to stdout";
        starch2stdout_stage.pid = 0;
        starch2stdout_stage.status = 0;
        starch2stdout_stage.buffer_size = buffer_size;
    }
    else {
        fprintf(stderr, "Error: Unknown conversion parameter combination\n");
        c2b_print_usage(stderr);
        exit(ENOTSUP); /* Operation not supported (POSIX.1) */
    }

    /*
       We open pid_t (process) instances to handle data in a specified order. 
    */

    c2b_cmd_cat_stdin(cat2generic_cmd);
#ifdef DEBUG
    fprintf(stderr, "Debug: c2b_cmd_cat_stdin: [%s]\n", cat2generic_cmd);
#endif

    generic2bed_unsorted_stage.pid = c2b_popen4(cat2generic_cmd,
                                                p->in[0],
                                                p->out[0],
                                                p->err[0],
                                                POPEN4_FLAG_NONE);

    if (waitpid(generic2bed_unsorted_stage.pid, 
                &generic2bed_unsorted_stage.status, 
                WNOHANG | WUNTRACED) == -1) {
        errsv = errno;
        fprintf(stderr, "Error: Generic stdin stage waitpid() call failed (%s)\n", (errsv == ECHILD ? "ECHILD" : (errsv == EINTR ? "EINTR" : "EINVAL")));
        exit(errsv);
    }

    if (c2b_globals.sort->is_enabled) {
        c2b_cmd_sort_bed(bed_unsorted2bed_sorted_cmd);
#ifdef DEBUG
        fprintf(stderr, "Debug: c2b_cmd_sort_bed: [%s]\n", bed_unsorted2bed_sorted_cmd);
#endif

        bed_unsorted2bed_sorted_stage.pid = c2b_popen4(bed_unsorted2bed_sorted_cmd,
                                                       p->in[2],
                                                       p->out[2],
                                                       p->err[2],
                                                       POPEN4_FLAG_NONE);

        if (waitpid(bed_unsorted2bed_sorted_stage.pid, 
                    &bed_unsorted2bed_sorted_stage.status, 
                    WNOHANG | WUNTRACED) == -1) {
            errsv = errno;
            fprintf(stderr, "Error: Sort stage waitpid() call failed (%s)\n", (errsv == ECHILD ? "ECHILD" : (errsv == EINTR ? "EINTR" : "EINVAL")));
            exit(errsv);
        }
    }

    if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        c2b_cmd_starch_bed(bed_sorted2starch_cmd);
#ifdef DEBUG
        fprintf(stderr, "Debug: c2b_cmd_starch_bed: [%s]\n", bed_sorted2starch_cmd);
#endif

        bed_sorted2starch_stage.pid = c2b_popen4(bed_sorted2starch_cmd,
                                                 p->in[3],
                                                 p->out[3],
                                                 p->err[3],
                                                 POPEN4_FLAG_NONE);

        if (waitpid(bed_sorted2starch_stage.pid, 
                    &bed_sorted2starch_stage.status, 
                    WNOHANG | WUNTRACED) == -1) {
            errsv = errno;
            fprintf(stderr, "Error: Compression stage waitpid() call failed (%s)\n", (errsv == ECHILD ? "ECHILD" : (errsv == EINTR ? "EINTR" : "EINVAL")));
            exit(errsv);
        }
    }

#ifdef DEBUG
    c2b_debug_pipeset(p, MAX_PIPES);
#endif

    /*
       Once we have the desired process instances, we create and join
       threads for their ordered execution.
    */

    if ((!c2b_globals.sort->is_enabled) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        pthread_create(&cat2generic_thread,
                       NULL,
                       c2b_read_bytes_from_stdin,
                       &cat2generic_stage);
        pthread_create(&generic2bed_unsorted_thread,
                       NULL,
                       c2b_process_intermediate_bytes_by_lines,
                       &generic2bed_unsorted_stage);
        pthread_create(&bed_unsorted2stdout_thread,
                       NULL,
                       c2b_write_in_bytes_to_stdout,
                       &bed_unsorted2stdout_stage);
    }
    else if (c2b_globals.output_format_idx == BED_FORMAT) {
        pthread_create(&cat2generic_thread,
                       NULL,
                       c2b_read_bytes_from_stdin,
                       &cat2generic_stage);
        pthread_create(&generic2bed_unsorted_thread,
                       NULL,
                       c2b_process_intermediate_bytes_by_lines,
                       &generic2bed_unsorted_stage);
        pthread_create(&bed_unsorted2bed_sorted_thread,
                       NULL,
                       c2b_write_in_bytes_to_in_process,
                       &bed_unsorted2bed_sorted_stage);
        pthread_create(&bed_sorted2stdout_thread,
                       NULL,
                       c2b_write_out_bytes_to_stdout,
                       &bed_sorted2stdout_stage);
    }
    else if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        pthread_create(&cat2generic_thread,
                       NULL,
                       c2b_read_bytes_from_stdin,
                       &cat2generic_stage);
        pthread_create(&generic2bed_unsorted_thread,
                       NULL,
                       c2b_process_intermediate_bytes_by_lines,
                       &generic2bed_unsorted_stage);
        pthread_create(&bed_unsorted2bed_sorted_thread,
                       NULL,
                       c2b_write_in_bytes_to_in_process,
                       &bed_unsorted2bed_sorted_stage);
        pthread_create(&bed_sorted2starch_thread,
                       NULL,
                       c2b_write_out_bytes_to_in_process,
                       &bed_sorted2starch_stage);
        pthread_create(&starch2stdout_thread,
                       NULL,
                       c2b_write_out_bytes_to_stdout,
                       &starch2stdout_stage);
    }

    if ((!c2b_globals.sort->is_enabled) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        pthread_join(cat2generic_thread, (void **) NULL);
        pthread_join(generic2bed_unsorted_thread, (void **) NULL);
        pthread_join(bed_unsorted2stdout_thread, (void **) NULL);
    }
    else if (c2b_globals.output_format_idx == BED_FORMAT) {
        pthread_join(cat2generic_thread, (void **) NULL);
        pthread_join(generic2bed_unsorted_thread, (void **) NULL);
        pthread_join(bed_unsorted2bed_sorted_thread, (void **) NULL);
        pthread_join(bed_sorted2stdout_thread, (void **) NULL);
    }
    else if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        pthread_join(cat2generic_thread, (void **) NULL);
        pthread_join(generic2bed_unsorted_thread, (void **) NULL);
        pthread_join(bed_unsorted2bed_sorted_thread, (void **) NULL);
        pthread_join(bed_sorted2starch_thread, (void **) NULL);
        pthread_join(starch2stdout_thread, (void **) NULL);
    }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_generic_conversion() - exit  ---\n");
#endif
}

static void
c2b_init_bam_conversion(c2b_pipeset_t* p)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_bam_conversion() - enter ---\n");
#endif

    pthread_t bam2sam_thread; 
    pthread_t sam2bed_unsorted_thread; 
    pthread_t bed_unsorted2stdout_thread;
    pthread_t bed_unsorted2bed_sorted_thread;
    pthread_t bed_sorted2stdout_thread;
    pthread_t bed_sorted2starch_thread;
    pthread_t starch2stdout_thread;
    c2b_pipeline_stage_t bam2sam_stage;
    c2b_pipeline_stage_t sam2bed_unsorted_stage;
    c2b_pipeline_stage_t bed_unsorted2stdout_stage;
    c2b_pipeline_stage_t bed_unsorted2bed_sorted_stage;
    c2b_pipeline_stage_t bed_sorted2stdout_stage;
    c2b_pipeline_stage_t bed_sorted2starch_stage;
    c2b_pipeline_stage_t starch2stdout_stage;
    char bam2sam_cmd[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    char bed_unsorted2bed_sorted_cmd[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    char bed_sorted2starch_cmd[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    void (*sam2bed_unsorted_line_functor)(char **, ssize_t *, ssize_t *, char *, ssize_t) = NULL;
    ssize_t buffer_size = C2B_THREAD_IO_BUFFER_SIZE;
    int errsv = errno;

    sam2bed_unsorted_line_functor = (!c2b_globals.split_flag ?
                                     &c2b_line_convert_sam_to_bed_unsorted_without_split_operation :
                                     &c2b_line_convert_sam_to_bed_unsorted_with_split_operation);

    if ((!c2b_globals.sort->is_enabled) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        bam2sam_stage.pipeset = p;
        bam2sam_stage.line_functor = NULL;
        bam2sam_stage.src = -1;
        bam2sam_stage.dest = 0;
        bam2sam_stage.description = "BAM data from stdin to SAM";
        bam2sam_stage.pid = 0;
        bam2sam_stage.status = 0;
        bam2sam_stage.buffer_size = buffer_size;
        
        sam2bed_unsorted_stage.pipeset = p;
        sam2bed_unsorted_stage.line_functor = sam2bed_unsorted_line_functor;
        sam2bed_unsorted_stage.src = 0;
        sam2bed_unsorted_stage.dest = 1;
        sam2bed_unsorted_stage.description = "SAM to unsorted BED";
        sam2bed_unsorted_stage.pid = 0;
        sam2bed_unsorted_stage.status = 0;
        sam2bed_unsorted_stage.buffer_size = buffer_size;

        bed_unsorted2stdout_stage.pipeset = p;
        bed_unsorted2stdout_stage.line_functor = NULL;
        bed_unsorted2stdout_stage.src = 1;
        bed_unsorted2stdout_stage.dest = -1;
        bed_unsorted2stdout_stage.description = "Unsorted BED to stdout";
        bed_unsorted2stdout_stage.pid = 0;
        bed_unsorted2stdout_stage.status = 0;
        bed_unsorted2stdout_stage.buffer_size = buffer_size;
    }
    else if (c2b_globals.output_format_idx == BED_FORMAT) {
        bam2sam_stage.pipeset = p;
        bam2sam_stage.line_functor = NULL;
        bam2sam_stage.src = -1;
        bam2sam_stage.dest = 0;
        bam2sam_stage.description = "BAM data from stdin to SAM";
        bam2sam_stage.pid = 0;
        bam2sam_stage.status = 0;
        bam2sam_stage.buffer_size = buffer_size;
        
        sam2bed_unsorted_stage.pipeset = p;
        sam2bed_unsorted_stage.line_functor = sam2bed_unsorted_line_functor;
        sam2bed_unsorted_stage.src = 0;
        sam2bed_unsorted_stage.dest = 1;
        sam2bed_unsorted_stage.description = "SAM to unsorted BED";
        sam2bed_unsorted_stage.pid = 0;
        sam2bed_unsorted_stage.status = 0;
        sam2bed_unsorted_stage.buffer_size = buffer_size;
        
        bed_unsorted2bed_sorted_stage.pipeset = p;
        bed_unsorted2bed_sorted_stage.line_functor = NULL;
        bed_unsorted2bed_sorted_stage.src = 1;
        bed_unsorted2bed_sorted_stage.dest = 2;
        bed_unsorted2bed_sorted_stage.description = "Unsorted BED to sorted BED";
        bed_unsorted2bed_sorted_stage.pid = 0;
        bed_unsorted2bed_sorted_stage.status = 0;
        bed_unsorted2bed_sorted_stage.buffer_size = buffer_size;

        bed_sorted2stdout_stage.pipeset = p;
        bed_sorted2stdout_stage.line_functor = NULL;
        bed_sorted2stdout_stage.src = 2;
        bed_sorted2stdout_stage.dest = -1;
        bed_sorted2stdout_stage.description = "Sorted BED to stdout";
        bed_sorted2stdout_stage.pid = 0;
        bed_sorted2stdout_stage.status = 0;
        bed_sorted2stdout_stage.buffer_size = buffer_size;
    }
    else if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        bam2sam_stage.pipeset = p;
        bam2sam_stage.line_functor = NULL;
        bam2sam_stage.src = -1;
        bam2sam_stage.dest = 0;
        bam2sam_stage.description = "BAM data from stdin to SAM";
        bam2sam_stage.pid = 0;
        bam2sam_stage.status = 0;
        bam2sam_stage.buffer_size = buffer_size;

        sam2bed_unsorted_stage.pipeset = p;
        sam2bed_unsorted_stage.line_functor = sam2bed_unsorted_line_functor;
        sam2bed_unsorted_stage.src = 0;
        sam2bed_unsorted_stage.dest = 1;
        sam2bed_unsorted_stage.description = "SAM to unsorted BED";
        sam2bed_unsorted_stage.pid = 0;
        sam2bed_unsorted_stage.status = 0;
        sam2bed_unsorted_stage.buffer_size = buffer_size;

        bed_unsorted2bed_sorted_stage.pipeset = p;
        bed_unsorted2bed_sorted_stage.line_functor = NULL;
        bed_unsorted2bed_sorted_stage.src = 1;
        bed_unsorted2bed_sorted_stage.dest = 2;
        bed_unsorted2bed_sorted_stage.description = "Unsorted BED to sorted BED";
        bed_unsorted2bed_sorted_stage.pid = 0;
        bed_unsorted2bed_sorted_stage.status = 0;
        bed_unsorted2bed_sorted_stage.buffer_size = buffer_size;

        bed_sorted2starch_stage.pipeset = p;
        bed_sorted2starch_stage.line_functor = NULL;
        bed_sorted2starch_stage.src = 2;
        bed_sorted2starch_stage.dest = 3;
        bed_sorted2starch_stage.description = "Sorted BED to Starch";
        bed_sorted2starch_stage.pid = 0;
        bed_sorted2starch_stage.status = 0;
        bed_sorted2starch_stage.buffer_size = buffer_size;

        starch2stdout_stage.pipeset = p;
        starch2stdout_stage.line_functor = NULL;
        starch2stdout_stage.src = 3;
        starch2stdout_stage.dest = -1;
        starch2stdout_stage.description = "Starch to stdout";
        starch2stdout_stage.pid = 0;
        starch2stdout_stage.status = 0;
        starch2stdout_stage.buffer_size = buffer_size;
    }
    else {
        fprintf(stderr, "Error: Unknown BAM conversion parameter combination\n");
        c2b_print_usage(stderr);
        exit(ENOTSUP); /* Operation not supported (POSIX.1) */
    }

    /*
       We open pid_t (process) instances to handle data in a specified order. 
    */

    c2b_cmd_bam_to_sam(bam2sam_cmd);
#ifdef DEBUG
    fprintf(stderr, "Debug: c2b_cmd_bam_to_sam: [%s]\n", bam2sam_cmd);
#endif

    bam2sam_stage.pid = c2b_popen4(bam2sam_cmd,
                                   p->in[0],
                                   p->out[0],
                                   p->err[0],
                                   POPEN4_FLAG_NONE);

    if (waitpid(bam2sam_stage.pid, 
                &bam2sam_stage.status, 
                WNOHANG | WUNTRACED) == -1) {
        errsv = errno;
        fprintf(stderr, "Error: BAM-to-SAM stage waitpid() call failed (%s)\n", (errsv == ECHILD ? "ECHILD" : (errsv == EINTR ? "EINTR" : "EINVAL")));
        exit(errsv);
    }

    if (c2b_globals.sort->is_enabled) {
        c2b_cmd_sort_bed(bed_unsorted2bed_sorted_cmd);
#ifdef DEBUG
        fprintf(stderr, "Debug: c2b_cmd_sort_bed: [%s]\n", bed_unsorted2bed_sorted_cmd);
#endif

        bed_unsorted2bed_sorted_stage.pid = c2b_popen4(bed_unsorted2bed_sorted_cmd,
                                                       p->in[2],
                                                       p->out[2],
                                                       p->err[2],
                                                       POPEN4_FLAG_NONE);
        
        if (waitpid(bed_unsorted2bed_sorted_stage.pid, 
                    &bed_unsorted2bed_sorted_stage.status, 
                    WNOHANG | WUNTRACED) == -1) {
            errsv = errno;
            fprintf(stderr, "Error: Sort stage waitpid() call failed (%s)\n", (errsv == ECHILD ? "ECHILD" : (errsv == EINTR ? "EINTR" : "EINVAL")));
            exit(errsv);
        }
    }

    if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        c2b_cmd_starch_bed(bed_sorted2starch_cmd);
#ifdef DEBUG
        fprintf(stderr, "Debug: c2b_cmd_starch_bed: [%s]\n", bed_sorted2starch_cmd);
#endif

        bed_sorted2starch_stage.pid = c2b_popen4(bed_sorted2starch_cmd,
                                                 p->in[3],
                                                 p->out[3],
                                                 p->err[3],
                                                 POPEN4_FLAG_NONE);

        if (waitpid(bed_sorted2starch_stage.pid, 
                    &bed_sorted2starch_stage.status, 
                    WNOHANG | WUNTRACED) == -1) {
            errsv = errno;
            fprintf(stderr, "Error: Compression stage waitpid() call failed (%s)\n", (errsv == ECHILD ? "ECHILD" : (errsv == EINTR ? "EINTR" : "EINVAL")));
            exit(errsv);
        }
    }

#ifdef DEBUG
    c2b_debug_pipeset(p, MAX_PIPES);
#endif

    /*
       Once we have the desired process instances, we create and join
       threads for their ordered execution.
    */

    if ((!c2b_globals.sort->is_enabled) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        pthread_create(&bam2sam_thread,
                       NULL,
                       c2b_read_bytes_from_stdin,
                       &bam2sam_stage);
        pthread_create(&sam2bed_unsorted_thread,
                       NULL,
                       c2b_process_intermediate_bytes_by_lines,
                       &sam2bed_unsorted_stage);
        pthread_create(&bed_unsorted2stdout_thread,
                       NULL,
                       c2b_write_in_bytes_to_stdout,
                       &bed_unsorted2stdout_stage);
    }
    else if (c2b_globals.output_format_idx == BED_FORMAT) {
        pthread_create(&bam2sam_thread,
                       NULL,
                       c2b_read_bytes_from_stdin,
                       &bam2sam_stage);
        pthread_create(&sam2bed_unsorted_thread,
                       NULL,
                       c2b_process_intermediate_bytes_by_lines,
                       &sam2bed_unsorted_stage);
        pthread_create(&bed_unsorted2bed_sorted_thread,
                       NULL,
                       c2b_write_in_bytes_to_in_process,
                       &bed_unsorted2bed_sorted_stage);
        pthread_create(&bed_sorted2stdout_thread,
                       NULL,
                       c2b_write_out_bytes_to_stdout,
                       &bed_sorted2stdout_stage);
    }
    else if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        pthread_create(&bam2sam_thread,
                       NULL,
                       c2b_read_bytes_from_stdin,
                       &bam2sam_stage);
        pthread_create(&sam2bed_unsorted_thread,
                       NULL,
                       c2b_process_intermediate_bytes_by_lines,
                       &sam2bed_unsorted_stage);
        pthread_create(&bed_unsorted2bed_sorted_thread,
                       NULL,
                       c2b_write_in_bytes_to_in_process,
                       &bed_unsorted2bed_sorted_stage);
        pthread_create(&bed_sorted2starch_thread,
                       NULL,
                       c2b_write_out_bytes_to_in_process,
                       &bed_sorted2starch_stage);
        pthread_create(&starch2stdout_thread,
                       NULL,
                       c2b_write_out_bytes_to_stdout,
                       &starch2stdout_stage);
    }

    if ((!c2b_globals.sort->is_enabled) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        pthread_join(bam2sam_thread, (void **) NULL);
        pthread_join(sam2bed_unsorted_thread, (void **) NULL);
        pthread_join(bed_unsorted2stdout_thread, (void **) NULL);
    }
    else if (c2b_globals.output_format_idx == BED_FORMAT) {
        pthread_join(bam2sam_thread, (void **) NULL);
        pthread_join(sam2bed_unsorted_thread, (void **) NULL);
        pthread_join(bed_unsorted2bed_sorted_thread, (void **) NULL);
        pthread_join(bed_sorted2stdout_thread, (void **) NULL);
    }
    else if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        pthread_join(bam2sam_thread, (void **) NULL);
        pthread_join(sam2bed_unsorted_thread, (void **) NULL);
        pthread_join(bed_unsorted2bed_sorted_thread, (void **) NULL);
        pthread_join(bed_sorted2starch_thread, (void **) NULL);
        pthread_join(starch2stdout_thread, (void **) NULL);
    }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_bam_conversion() - exit  ---\n");
#endif
}

static inline void
c2b_cmd_cat_stdin(char* cmd)
{
    const char* cat_args = " - ";
    
    /* /path/to/cat - */
    memcpy(cmd,
           c2b_globals.cat->path,
           strlen(c2b_globals.cat->path) + 1);
    memcpy(cmd + strlen(c2b_globals.cat->path),
           cat_args,
           strlen(cat_args) + 1);
    cmd[strlen(c2b_globals.cat->path) + strlen(cat_args)] = '\0';
}

static inline void
c2b_cmd_bam_to_sam(char* cmd)
{
    const char* bam2sam_args = " view -h -";

    /* /path/to/samtools view -h - */
    memcpy(cmd, 
           c2b_globals.sam->samtools_path, 
           strlen(c2b_globals.sam->samtools_path) + 1);
    memcpy(cmd + strlen(c2b_globals.sam->samtools_path), 
           bam2sam_args, 
           strlen(bam2sam_args) + 1);
    cmd[strlen(c2b_globals.sam->samtools_path) + strlen(bam2sam_args)] = '\0';
}

static inline void
c2b_cmd_sort_bed(char* cmd)
{
    char sort_bed_args[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    memset(sort_bed_args, 0, C2B_MAX_COMMAND_LINE_LENGTH_VALUE);

    /* /path/to/sort-bed [--max-mem <val>] [--tmpdir <path>] - */
    if (c2b_globals.sort->max_mem_value) {
        memcpy(sort_bed_args,
               sort_bed_max_mem_arg, 
               strlen(sort_bed_max_mem_arg) + 1);
        memcpy(sort_bed_args + strlen(sort_bed_args), 
               c2b_globals.sort->max_mem_value, 
               strlen(c2b_globals.sort->max_mem_value) + 1);
        sort_bed_args[strlen(sort_bed_max_mem_arg) + strlen(sort_bed_args)] = '\0';
    }
    else {
        memcpy(sort_bed_args, 
               sort_bed_max_mem_default_arg, 
               strlen(sort_bed_max_mem_default_arg) + 1);
        sort_bed_args[strlen(sort_bed_max_mem_default_arg)] = '\0';
    }
    if (c2b_globals.sort->sort_tmpdir_path) {
        memcpy(sort_bed_args + strlen(sort_bed_args),
               sort_bed_tmpdir_arg,
               strlen(sort_bed_tmpdir_arg) + 1);
        memcpy(sort_bed_args + strlen(sort_bed_args),
               c2b_globals.sort->sort_tmpdir_path,
               strlen(c2b_globals.sort->sort_tmpdir_path) + 1);
        sort_bed_args[strlen(sort_bed_args) + strlen(c2b_globals.sort->sort_tmpdir_path)] = '\0';
    }
    memcpy(sort_bed_args + strlen(sort_bed_args),
           sort_bed_stdin,
           strlen(sort_bed_stdin) + 1);
    sort_bed_args[strlen(sort_bed_args) + strlen(sort_bed_stdin)] = '\0';

    /* cmd */
    memcpy(cmd, 
           c2b_globals.sort->sort_bed_path, 
           strlen(c2b_globals.sort->sort_bed_path) + 1);
    memcpy(cmd + strlen(c2b_globals.sort->sort_bed_path), 
           sort_bed_args, 
           strlen(sort_bed_args) + 1);
    cmd[strlen(c2b_globals.sort->sort_bed_path) + strlen(sort_bed_args)] = '\0';
}

static inline void
c2b_cmd_starch_bed(char* cmd) 
{
    char starch_args[C2B_MAX_COMMAND_LINE_LENGTH_VALUE];
    memset(starch_args, 0, C2B_MAX_COMMAND_LINE_LENGTH_VALUE);

    /* /path/to/starch [--bzip2 | --gzip] [--note="xyz..."] - */
    if (c2b_globals.starch->bzip2) {
        memcpy(starch_args,
               starch_bzip2_arg,
               strlen(starch_bzip2_arg) + 1);
        starch_args[strlen(starch_bzip2_arg)] = '\0';
    }
    else if (c2b_globals.starch->gzip) {
        memcpy(starch_args,
               starch_gzip_arg,
               strlen(starch_gzip_arg) + 1);
        starch_args[strlen(starch_gzip_arg)] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "Debug: c2b_globals.starch->bzip2: [%d]\n", c2b_globals.starch->bzip2);
    fprintf(stderr, "Debug: c2b_globals.starch->gzip:  [%d]\n", c2b_globals.starch->gzip);
    fprintf(stderr, "Debug: c2b_globals.starch->note:  [%s]\n", c2b_globals.starch->note);
    fprintf(stderr, "Debug: starch_args:               [%s]\n", starch_args);
#endif

    if (c2b_globals.starch->note) {
        memcpy(starch_args + strlen(starch_args),
               starch_note_prefix_arg,
               strlen(starch_note_prefix_arg) + 1);
        memcpy(starch_args + strlen(starch_args),
               c2b_globals.starch->note,
               strlen(c2b_globals.starch->note) + 1);
        memcpy(starch_args + strlen(starch_args),
               starch_note_suffix_arg,
               strlen(starch_note_suffix_arg) + 1);
        starch_args[strlen(starch_args) + strlen(starch_note_prefix_arg) + strlen(c2b_globals.starch->note) + strlen(starch_note_suffix_arg)] = '\0';
    }
    memcpy(starch_args + strlen(starch_args),
           starch_stdin_arg,
           strlen(starch_stdin_arg) + 1);

#ifdef DEBUG
    fprintf(stderr, "Debug: starch_args: [%s]\n", starch_args);
#endif

    /* cmd */
    memcpy(cmd, 
           c2b_globals.starch->path, 
           strlen(c2b_globals.starch->path) + 1);
    memcpy(cmd + strlen(c2b_globals.starch->path), 
           starch_args, 
           strlen(starch_args) + 1);
    cmd[strlen(c2b_globals.starch->path) + strlen(starch_args)] = '\0';
}

static void
c2b_gtf_init_element(c2b_gtf_t** e)
{
    *e = malloc(sizeof(c2b_gtf_t));
    if (!*e) {
        fprintf(stderr, "Error: Could not allocate space for GTF element pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    (*e)->seqname = NULL, (*e)->seqname = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->seqname)));
    if (!(*e)->seqname) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element seqname malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->seqname_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->source = NULL, (*e)->source = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->source)));
    if (!(*e)->source) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element source malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->source_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->feature = NULL, (*e)->feature = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->feature)));
    if (!(*e)->feature) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element feature malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->feature_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->start = 0;
    (*e)->start_str = NULL, (*e)->start_str = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->start_str)));
    if (!(*e)->start_str) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element start string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->start_str_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->end = 0;
    (*e)->end_str = NULL, (*e)->end_str = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->end_str)));
    if (!(*e)->end_str) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element end string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->end_str_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->score = NULL, (*e)->score = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->score)));
    if (!(*e)->score) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element score malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->score_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->strand = NULL, (*e)->strand = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->strand)));
    if (!(*e)->strand) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element strand malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->strand_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->frame = NULL, (*e)->frame = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->frame)));
    if (!(*e)->frame) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element frame malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->frame_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->attributes = NULL, (*e)->attributes = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->attributes)));
    if (!(*e)->attributes) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element attributes malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->attributes_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->id = NULL, (*e)->id = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->id)));
    if (!(*e)->id) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element id malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->id_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->comments = NULL, (*e)->comments = malloc(C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->comments)));
    if (!(*e)->comments) { 
        fprintf(stderr, "Error: Could not allocate space for GTF element comments malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->comments_capacity = C2B_GTF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;
}

static void
c2b_gtf_delete_element(c2b_gtf_t* e)
{
    if (e->seqname)         { free(e->seqname),         e->seqname = NULL;         }
    if (e->source)          { free(e->source),          e->source = NULL;          }
    if (e->feature)         { free(e->feature),         e->feature = NULL;         }
    if (e->start_str)       { free(e->start_str),       e->start_str = NULL;       }
    if (e->end_str)         { free(e->end_str),         e->end_str = NULL;         }
    if (e->score)           { free(e->score),           e->score = NULL;           }
    if (e->strand)          { free(e->strand),          e->strand = NULL;          }
    if (e->frame)           { free(e->frame),           e->frame = NULL;           }
    if (e->attributes)      { free(e->attributes),      e->attributes = NULL;      }
    if (e->id)              { free(e->id),              e->id = NULL;              }
    if (e->comments)        { free(e->comments),        e->comments = NULL;        }
    if (e)                  { free(e),                  e = NULL;                  }
}

static void
c2b_line_convert_gtf_to_bed_unsorted(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    ssize_t gtf_field_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    int gtf_field_idx = 0;
    ssize_t current_src_posn = -1;

    while (++current_src_posn < src_size) {
        if ((src[current_src_posn] == c2b_tab_delim) || (src[current_src_posn] == c2b_line_delim)) {
            gtf_field_offsets[gtf_field_idx++] = current_src_posn;
        }
        if (gtf_field_idx >= C2B_MAX_FIELD_COUNT_VALUE) {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", gtf_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }
    gtf_field_offsets[gtf_field_idx] = src_size;
    gtf_field_offsets[gtf_field_idx + 1] = -1;

    /* 
       If number of fields is not in bounds, we may need to exit early
    */

    if (((gtf_field_idx + 1) < c2b_gtf_field_min) || ((gtf_field_idx + 1) > c2b_gtf_field_max)) {
        if (gtf_field_idx == 0) {
            if (src[0] == c2b_gtf_comment) {
                if (!c2b_globals.keep_header_flag) {
                    return;
                }
                else {
                    /* set up temporary header stream buffers */
                    if (!c2b_globals.src_line_str) {
                        c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                        if (!c2b_globals.src_line_str) {
                            fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                            exit(ENOMEM);
                        }
                    }
                    if (!c2b_globals.dest_line_str) {
                        c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                        if (!c2b_globals.dest_line_str) {
                            fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                            exit(ENOMEM);
                        }
                    }
                    /* copy header line to destination stream buffer */
                    memcpy(c2b_globals.src_line_str, src, src_size);
                    c2b_globals.src_line_str[src_size] = '\0';
                    sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
                    memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
                    *dest_size += strlen(c2b_globals.dest_line_str);
                    c2b_globals.header_line_idx++;
                    return;
                }
            }
            else {
                return;
            }
        }
        else {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may not match input format\n", gtf_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }

    /* 0 - seqname */
    ssize_t seqname_size = gtf_field_offsets[0];
    if (seqname_size >= c2b_globals.gtf->element->seqname_capacity) {
        char *seqname_resized = NULL;
        seqname_resized = realloc(c2b_globals.gtf->element->seqname, seqname_size + 1);
        if (seqname_resized) {
            c2b_globals.gtf->element->seqname = seqname_resized;
            c2b_globals.gtf->element->seqname_capacity = seqname_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SEQNAME string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (seqname_size > 0) {
        memcpy(c2b_globals.gtf->element->seqname, src, seqname_size);
        c2b_globals.gtf->element->seqname[seqname_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->seqname[0] = '\0';
    }

    /* 1 - source */
    ssize_t source_size = gtf_field_offsets[1] - gtf_field_offsets[0] - 1;
    if (source_size >= c2b_globals.gtf->element->source_capacity) {
        char *source_resized = NULL;
        source_resized = realloc(c2b_globals.gtf->element->source, source_size + 1);
        if (source_resized) {
            c2b_globals.gtf->element->source = source_resized;
            c2b_globals.gtf->element->source_capacity = source_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SOURCE string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (source_size > 0) {
        memcpy(c2b_globals.gtf->element->source, src + gtf_field_offsets[0] + 1, source_size);
        c2b_globals.gtf->element->source[source_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->source[0] = '\0';
    }

    /* 2 - feature */
    ssize_t feature_size = gtf_field_offsets[2] - gtf_field_offsets[1] - 1;
    if (feature_size >= c2b_globals.gtf->element->feature_capacity) {
        char *feature_resized = NULL;
        feature_resized = realloc(c2b_globals.gtf->element->feature, feature_size + 1);
        if (feature_resized) {
            c2b_globals.gtf->element->feature = feature_resized;
            c2b_globals.gtf->element->feature_capacity = feature_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize FEATURE string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (feature_size > 0) {
        memcpy(c2b_globals.gtf->element->feature, src + gtf_field_offsets[1] + 1, feature_size);
        c2b_globals.gtf->element->feature[feature_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->feature[0] = '\0';
    }

    /* 3 - start */
    ssize_t start_size = gtf_field_offsets[3] - gtf_field_offsets[2] - 1;
    if (start_size >= c2b_globals.gtf->element->start_str_capacity) {
        char *start_str_resized = NULL;
        start_str_resized = realloc(c2b_globals.gtf->element->start_str, start_size + 1);
        if (start_str_resized) {
            c2b_globals.gtf->element->start_str = start_str_resized;
            c2b_globals.gtf->element->start_str_capacity = start_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize start string buffer in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (start_size > 0) {
        memcpy(c2b_globals.gtf->element->start_str, src + gtf_field_offsets[2] + 1, start_size);
        c2b_globals.gtf->element->start_str[start_size] = '\0';
        c2b_globals.gtf->element->start = strtoull(c2b_globals.gtf->element->start_str, NULL, 10);
    }
    else {
        c2b_globals.gtf->element->start_str[0] = '\0';
        c2b_globals.gtf->element->start = 0;
    }

    /* 4 - end */
    ssize_t end_size = gtf_field_offsets[4] - gtf_field_offsets[3] - 1;
    if (end_size >= c2b_globals.gtf->element->end_str_capacity) {
        char *end_str_resized = NULL;
        end_str_resized = realloc(c2b_globals.gtf->element->end_str, end_size + 1);
        if (end_str_resized) {
            c2b_globals.gtf->element->end_str = end_str_resized;
            c2b_globals.gtf->element->end_str_capacity = end_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize end string buffer in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (end_size > 0) {
        memcpy(c2b_globals.gtf->element->end_str, src + gtf_field_offsets[3] + 1, end_size);
        c2b_globals.gtf->element->end_str[end_size] = '\0';
        c2b_globals.gtf->element->end = strtoull(c2b_globals.gtf->element->end_str, NULL, 10);
    }
    else {
        c2b_globals.gtf->element->end_str[0] = '\0';
        c2b_globals.gtf->element->end = 1;
    }    

    /* 5 - score */
    ssize_t score_size = gtf_field_offsets[5] - gtf_field_offsets[4] - 1;
    if (score_size >= c2b_globals.gtf->element->score_capacity) {
        char *score_resized = NULL;
        score_resized = realloc(c2b_globals.gtf->element->score, score_size + 1);
        if (score_resized) {
            c2b_globals.gtf->element->score = score_resized;
            c2b_globals.gtf->element->score_capacity = score_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SCORE string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (score_size > 0) {
        memcpy(c2b_globals.gtf->element->score, src + gtf_field_offsets[4] + 1, score_size);
        c2b_globals.gtf->element->score[score_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->score[0] = '\0';
    }

    /* 6 - strand */
    ssize_t strand_size = gtf_field_offsets[6] - gtf_field_offsets[5] - 1;
    if (strand_size >= c2b_globals.gtf->element->strand_capacity) {
        char *strand_resized = NULL;
        strand_resized = realloc(c2b_globals.gtf->element->strand, strand_size + 1);
        if (strand_resized) {
            c2b_globals.gtf->element->strand = strand_resized;
            c2b_globals.gtf->element->strand_capacity = strand_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize STRAND string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (strand_size > 0) {
        memcpy(c2b_globals.gtf->element->strand, src + gtf_field_offsets[5] + 1, strand_size);
        c2b_globals.gtf->element->strand[strand_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->strand[0] = '\0';
    }

    /* 7 - frame */
    ssize_t frame_size = gtf_field_offsets[7] - gtf_field_offsets[6] - 1;
    if (frame_size >= c2b_globals.gtf->element->frame_capacity) {
        char *frame_resized = NULL;
        frame_resized = realloc(c2b_globals.gtf->element->frame, frame_size + 1);
        if (frame_resized) {
            c2b_globals.gtf->element->frame = frame_resized;
            c2b_globals.gtf->element->frame_capacity = frame_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize FRAME string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (frame_size > 0) {
        memcpy(c2b_globals.gtf->element->frame, src + gtf_field_offsets[6] + 1, frame_size);
        c2b_globals.gtf->element->frame[frame_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->frame[0] = '\0';
    }

    /* 8 - attributes */
    ssize_t attributes_size = gtf_field_offsets[8] - gtf_field_offsets[7] - 1;
    if (attributes_size >= c2b_globals.gtf->element->attributes_capacity) {
        char *attributes_resized = NULL;
        attributes_resized = realloc(c2b_globals.gtf->element->attributes, attributes_size * 2);
        if (attributes_resized) {
            c2b_globals.gtf->element->attributes = attributes_resized;
            c2b_globals.gtf->element->attributes_capacity = attributes_size * 2;
        }
        else {
            fprintf(stderr, "Error: Could not resize ATTRIBUTES string in GTF element struct\n");
            exit(ENOMEM);
        }
    }
    if (attributes_size > 0) {
        memcpy(c2b_globals.gtf->element->attributes, src + gtf_field_offsets[7] + 1, attributes_size);
        c2b_globals.gtf->element->attributes[attributes_size] = '\0';
    }
    else {
        c2b_globals.gtf->element->attributes[0] = '\0';
    }

    /* 9 - comments */
    ssize_t comments_size = 0;
    if (gtf_field_idx == 9) {
        comments_size = gtf_field_offsets[9] - gtf_field_offsets[8] - 1;
        if (comments_size >= c2b_globals.gtf->element->comments_capacity) {
            char *comments_resized = NULL;
            comments_resized = realloc(c2b_globals.gtf->element->comments, comments_size + 1);
            if (comments_resized) {
                c2b_globals.gtf->element->comments = comments_resized;
                c2b_globals.gtf->element->comments_capacity = comments_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize COMMENTS string in GTF element struct\n");
                exit(ENOMEM);
            }
        }
        if (comments_size > 0) {
            memcpy(c2b_globals.gtf->element->comments, src + gtf_field_offsets[8] + 1, comments_size);
            c2b_globals.gtf->element->comments[comments_size] = '\0';
        }
        else {
            c2b_globals.gtf->element->comments[0] = '\0';
        }
    }

    /* 
       Fix coordinate indexing, and (if needed) add attribute for zero-length record
    */

    if (c2b_globals.gtf->element->start == c2b_globals.gtf->element->end) {
        c2b_globals.gtf->element->start -= 1;
        ssize_t trailing_semicolon_fudge = (c2b_globals.gtf->element->attributes[strlen(c2b_globals.gtf->element->attributes) - 1] == ';') ? 1 : 0;
        memcpy(c2b_globals.gtf->element->attributes + strlen(c2b_globals.gtf->element->attributes) - trailing_semicolon_fudge, 
               c2b_gtf_zero_length_insertion_attribute, 
               strlen(c2b_gtf_zero_length_insertion_attribute) + 1);
    }
    else {
        c2b_globals.gtf->element->start -= 1;
    }

    /* 
       Parse ID value out from attributes string
    */

    char *attributes_copy = NULL;
    attributes_copy = malloc(strlen(c2b_globals.gtf->element->attributes) + 1);
    if (!attributes_copy) {
        fprintf(stderr, "Error: Could not allocate space for GTF attributes copy\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    memcpy(attributes_copy, c2b_globals.gtf->element->attributes, strlen(c2b_globals.gtf->element->attributes) + 1);
    const char *kv_tok;
    char *gene_id_str = NULL;
    char *transcript_id_str = NULL;
    boolean gene_id_value_defined = kFalse;
    boolean transcript_id_value_defined = kFalse;
    while((kv_tok = c2b_strsep(&attributes_copy, c2b_gtf_field_delimiter)) != NULL) {
        gene_id_str = strstr(kv_tok, c2b_gtf_gene_id_prefix);
        if (gene_id_str) {
            /* we remove quotation marks around ID string value */
            char *gtf_id_start = NULL;
            gtf_id_start = strchr(kv_tok + strlen(c2b_gtf_gene_id_prefix), c2b_gtf_id_delimiter);
            if (gtf_id_start - (kv_tok + strlen(c2b_gtf_gene_id_prefix)) < 0) {
                gtf_id_start = NULL;
            }
            char *gtf_id_end = NULL;
            if (gtf_id_start) {
                gtf_id_end = strchr(gtf_id_start + 1, c2b_gtf_id_delimiter);
                if (gtf_id_end - (kv_tok + strlen(c2b_gtf_gene_id_prefix)) <= 0) {
                    gtf_id_end = NULL;
                }
            }
            if (!gtf_id_start || !gtf_id_end) {
                fprintf(stderr, "Error: Could not parse ID from GTF attributes (malformed GTF at line [%" PRIu64 "]?)\n", c2b_globals.gtf->line_count + 1);
                exit(ENODATA); /* No data available (POSIX.1) */
            }
            if ((gtf_id_start && gtf_id_end) && (gtf_id_start != gtf_id_end)) {
                ssize_t id_size = gtf_id_end - gtf_id_start - 1;
                if (id_size >= c2b_globals.gtf->element->id_capacity) {
                    char *id_resized = NULL;
                    id_resized = realloc(c2b_globals.gtf->element->id, id_size + 1);
                    if (id_resized) {
                        c2b_globals.gtf->element->id = id_resized;
                        c2b_globals.gtf->element->id_capacity = id_size + 1;
                    }
                    else {
                        fprintf(stderr, "Error: Could not resize ID string in GTF element struct\n");
                        exit(ENOMEM);
                    }
                }
                memcpy(c2b_globals.gtf->element->id, gtf_id_start + 1, id_size);
                c2b_globals.gtf->element->id[id_size] = '\0';
            }
            else {
                c2b_globals.gtf->element->id[0] = '\0';
            }
            if (strlen(c2b_globals.gtf->element->id) == 0) {
                strcpy(c2b_globals.gtf->element->id, c2b_gtf_field_placeholder);
            }
            gene_id_value_defined = kTrue;
        }
        transcript_id_str = strstr(kv_tok, c2b_gtf_transcript_id_prefix);
        if (transcript_id_str) {
            transcript_id_value_defined = kTrue;
        }
    }
    if (!gene_id_value_defined || !transcript_id_value_defined) {
        fprintf(stderr, "Error: Potentially missing gene or transcript ID from GTF attributes (malformed GTF at line [%" PRIu64 "]?)\n", c2b_globals.gtf->line_count + 1);
        exit(ENODATA); /* No data available (POSIX.1) */        
    }
    free(attributes_copy), attributes_copy = NULL;

    /* 
       Convert GTF struct to BED string and copy it to destination
    */

    c2b_line_convert_gtf_ptr_to_bed(c2b_globals.gtf->element, dest, dest_size, dest_capacity);
    c2b_globals.gtf->line_count++;
}

static inline void
c2b_line_convert_gtf_ptr_to_bed(c2b_gtf_t* g, char** dest_line_ptr, ssize_t* dest_size, ssize_t* dest_capacity)
{
    /* 
       For GTF-formatted data, we use the mapping provided by BEDOPS convention described at:

       http://bedops.readthedocs.io/en/latest/content/reference/file-management/conversion/gtf2bed.html

       GTF field                 BED column index       BED field
       -------------------------------------------------------------------------
       seqname                   1                      chromosome
       start                     2                      start
       end                       3                      stop
       ID (via attributes)       4                      id
       score                     5                      score
       strand                    6                      strand

       The remaining GTF columns are mapped as-is, in same order, to adjacent BED columns:

       GTF field                 BED column index       BED field
       -------------------------------------------------------------------------
       source                    7                      -
       feature                   8                      -
       frame                     9                      -
       attributes                10                     -

       If present in the GTF2.2 input, the following column is also mapped:

       GTF field                 BED column index       BED field
       -------------------------------------------------------------------------
       comments                  11                     -
    */

    if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
        char *dest_line_resized = NULL;
        dest_line_resized = realloc(*dest_line_ptr, *dest_capacity * 2);
        if (dest_line_resized) {
            *dest_capacity *= 2;
            *dest_line_ptr = dest_line_resized;
        }
        else {
            fprintf(stderr, "Error: Could not resize dest_line_ptr string in GTF pointer conversion fn\n");
            exit(ENOMEM);
        }
    }
    
    if (strlen(g->seqname) == 0) {
        return;
    }

    if (strlen(g->comments) == 0) {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              g->seqname,
                              g->start,
                              g->end,
                              g->id,
                              g->score,
                              g->strand,
                              g->source,
                              g->feature,
                              g->frame,
                              g->attributes);
    }
    else {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              g->seqname,
                              g->start,
                              g->end,
                              g->id,
                              g->score,
                              g->strand,
                              g->source,
                              g->feature,
                              g->frame,
                              g->attributes,
                              g->comments);
    }
}

static void
c2b_gff_init_element(c2b_gff_t** e)
{
    *e = malloc(sizeof(c2b_gff_t));
    if (!*e) {
        fprintf(stderr, "Error: Could not allocate space for GFF element pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    (*e)->header = NULL, (*e)->header = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->header)));
    if (!(*e)->header) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element header buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->header_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;
    
    (*e)->seqid = NULL, (*e)->seqid = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->seqid)));
    if (!(*e)->seqid) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element seqid malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->seqid_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->source = NULL, (*e)->source = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->source)));
    if (!(*e)->source) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element source malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->source_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->type = NULL, (*e)->type = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->type)));
    if (!(*e)->type) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element type malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->type_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->start = 0;
    (*e)->start_str = NULL, (*e)->start_str = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->start_str)));
    if (!(*e)->start_str) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element start string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->start_str_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->end = 0;
    (*e)->end_str = NULL, (*e)->end_str = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->end_str)));
    if (!(*e)->end_str) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element end string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->end_str_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->score = NULL, (*e)->score = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->score)));
    if (!(*e)->score) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element score malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->score_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->strand = NULL, (*e)->strand = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->strand)));
    if (!(*e)->strand) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element strand malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->strand_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->phase = NULL, (*e)->phase = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->phase)));
    if (!(*e)->phase) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element phase malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->phase_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->attributes = NULL, (*e)->attributes = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->attributes)));
    if (!(*e)->attributes) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element attributes malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->attributes_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->id = NULL, (*e)->id = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->id)));
    if (!(*e)->id) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element id malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->id_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->non_int_prefix = NULL, (*e)->non_int_prefix = malloc(C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->non_int_prefix)));
    if (!(*e)->non_int_prefix) { 
        fprintf(stderr, "Error: Could not allocate space for GFF element non_int_prefix malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->non_int_prefix_capacity = C2B_GFF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;
}

static void
c2b_gff_delete_element(c2b_gff_t* e)
{
    if (e->header)          { free(e->header),          e->header = NULL;          }
    if (e->seqid)           { free(e->seqid),           e->seqid = NULL;           }
    if (e->source)          { free(e->source),          e->source = NULL;          }
    if (e->type)            { free(e->type),            e->type = NULL;            }
    if (e->start_str)       { free(e->start_str),       e->start_str = NULL;       }
    if (e->end_str)         { free(e->end_str),         e->end_str = NULL;         }
    if (e->score)           { free(e->score),           e->score = NULL;           }
    if (e->strand)          { free(e->strand),          e->strand = NULL;          }
    if (e->phase)           { free(e->phase),           e->phase = NULL;           }
    if (e->attributes)      { free(e->attributes),      e->attributes = NULL;      }
    if (e->id)              { free(e->id),              e->id = NULL;              }
    if (e->non_int_prefix)  { free(e->non_int_prefix),  e->non_int_prefix = NULL;  }
    if (e)                  { free(e),                  e = NULL;                  }
}

static void
c2b_line_convert_gff_to_bed_unsorted(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    ssize_t gff_field_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    int gff_field_idx = 0;
    ssize_t current_src_posn = -1;

    while (++current_src_posn < src_size) {
        if ((src[current_src_posn] == c2b_tab_delim) || (src[current_src_posn] == c2b_line_delim)) {
            gff_field_offsets[gff_field_idx++] = current_src_posn;
        }
        if (gff_field_idx >= C2B_MAX_FIELD_COUNT_VALUE) {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", gff_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }
    gff_field_offsets[gff_field_idx] = src_size;
    gff_field_offsets[gff_field_idx + 1] = -1;

    /* 
       If number of fields is not in bounds, we may need to exit early
    */

    if (((gff_field_idx + 1) < c2b_gff_field_min) || ((gff_field_idx + 1) > c2b_gff_field_max)) {
        ssize_t header_size = gff_field_offsets[0];
        if (header_size >= c2b_globals.gff->element->header_capacity) {
            char *header_resized = NULL;
            header_resized = realloc(c2b_globals.gff->element->header, header_size + 1);
            if (header_resized) {
                c2b_globals.gff->element->header = header_resized;
                c2b_globals.gff->element->header_capacity = header_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize start header buffer in GFF element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.gff->element->header, src, header_size);
        c2b_globals.gff->element->header[header_size] = '\0';

        ssize_t non_int_prefix_size = 3;
        if (non_int_prefix_size >= c2b_globals.gff->element->non_int_prefix_capacity) {
            char *non_int_prefix_resized = NULL;
            non_int_prefix_resized = realloc(c2b_globals.gff->element->non_int_prefix, non_int_prefix_size + 1);
            if (non_int_prefix_resized) {
                c2b_globals.gff->element->non_int_prefix = non_int_prefix_resized;
                c2b_globals.gff->element->non_int_prefix_capacity = non_int_prefix_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize non_int_prefix string buffer in GFF element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.gff->element->non_int_prefix, c2b_globals.gff->element->header, non_int_prefix_size - 1);
        c2b_globals.gff->element->non_int_prefix[non_int_prefix_size] = '\0';

        /* We compare against either of two standard GFF3 or GVF header pragmas */
        if ((strcmp(c2b_globals.gff->element->header, c2b_gff_header) == 0) ||
            (strcmp(c2b_globals.gff->element->header, c2b_gff_fasta) == 0) ||
            (strcmp(c2b_globals.gff->element->header, c2b_gvf_header) == 0) ||
            (strcmp(c2b_globals.gff->element->non_int_prefix, c2b_gvf_generic_header) == 0)) {
            if (!c2b_globals.keep_header_flag) {
                return;
            }
            else {
                /* set up temporary header stream buffers */
                if (!c2b_globals.src_line_str) {
                    c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                    if (!c2b_globals.src_line_str) {
                        fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                        exit(ENOMEM);
                    }
                }
                if (!c2b_globals.dest_line_str) {
                    c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                    if (!c2b_globals.dest_line_str) {
                        fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                        exit(ENOMEM);
                    }
                }
                /* copy header line to destination stream buffer */
                memcpy(c2b_globals.src_line_str, src, src_size);
                c2b_globals.src_line_str[src_size] = '\0';
                sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
                memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
                *dest_size += strlen(c2b_globals.dest_line_str);
                c2b_globals.header_line_idx++;
                return;
            }
        }
        else {
            return;
        }
    }

    /* 0 - seqid */
    ssize_t seqid_size = gff_field_offsets[0];
    if (seqid_size >= c2b_globals.gff->element->seqid_capacity) {
        char *seqid_resized = NULL;
        seqid_resized = realloc(c2b_globals.gff->element->seqid, seqid_size + 1);
        if (seqid_resized) {
            c2b_globals.gff->element->seqid = seqid_resized;
            c2b_globals.gff->element->seqid_capacity = seqid_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SEQID string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (seqid_size > 0) {
        memcpy(c2b_globals.gff->element->seqid, src, seqid_size);
        c2b_globals.gff->element->seqid[seqid_size] = '\0';
    }
    else {
        c2b_globals.gff->element->seqid[0] = '\0';
    }

    /* 1 - source */
    ssize_t source_size = gff_field_offsets[1] - gff_field_offsets[0] - 1;
    if (source_size >= c2b_globals.gff->element->source_capacity) {
        char *source_resized = NULL;
        source_resized = realloc(c2b_globals.gff->element->source, source_size + 1);
        if (source_resized) {
            c2b_globals.gff->element->source = source_resized;
            c2b_globals.gff->element->source_capacity = source_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SOURCE string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (source_size > 0) {
        memcpy(c2b_globals.gff->element->source, src + gff_field_offsets[0] + 1, source_size);
        c2b_globals.gff->element->source[source_size] = '\0';
    }
    else {
        c2b_globals.gff->element->source[0] = '\0';
    }

    /* 2 - type */
    ssize_t type_size = gff_field_offsets[2] - gff_field_offsets[1] - 1;
    if (type_size >= c2b_globals.gff->element->type_capacity) {
        char *type_resized = NULL;
        type_resized = realloc(c2b_globals.gff->element->type, type_size + 1);
        if (type_resized) {
            c2b_globals.gff->element->type = type_resized;
            c2b_globals.gff->element->type_capacity = type_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize TYPE string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (type_size > 0) {
        memcpy(c2b_globals.gff->element->type, src + gff_field_offsets[1] + 1, type_size);
        c2b_globals.gff->element->type[type_size] = '\0';
    }
    else {
        c2b_globals.gff->element->type[0] = '\0';
    }

    /* 3 - start */
    ssize_t start_size = gff_field_offsets[3] - gff_field_offsets[2] - 1;
    if (start_size >= c2b_globals.gff->element->start_str_capacity) {
        char *start_str_resized = NULL;
        start_str_resized = realloc(c2b_globals.gff->element->start_str, start_size + 1);
        if (start_str_resized) {
            c2b_globals.gff->element->start_str = start_str_resized;
            c2b_globals.gff->element->start_str_capacity = start_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize start string buffer in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (start_size > 0) {
        memcpy(c2b_globals.gff->element->start_str, src + gff_field_offsets[2] + 1, start_size);
        c2b_globals.gff->element->start_str[start_size] = '\0';
        c2b_globals.gff->element->start = strtoull(c2b_globals.gff->element->start_str, NULL, 10);
    }
    else {
        c2b_globals.gff->element->start_str[0] = '\0';
        c2b_globals.gff->element->start = 1;
    }

    /* 4 - end */
    ssize_t end_size = gff_field_offsets[4] - gff_field_offsets[3] - 1;
    if (end_size >= c2b_globals.gff->element->end_str_capacity) {
        char *end_str_resized = NULL;
        end_str_resized = realloc(c2b_globals.gff->element->end_str, end_size + 1);
        if (end_str_resized) {
            c2b_globals.gff->element->end_str = end_str_resized;
            c2b_globals.gff->element->end_str_capacity = end_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize end string buffer in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (end_size > 0) {
        memcpy(c2b_globals.gff->element->end_str, src + gff_field_offsets[3] + 1, end_size);
        c2b_globals.gff->element->end_str[end_size] = '\0';
        c2b_globals.gff->element->end = strtoull(c2b_globals.gff->element->end_str, NULL, 10);
    }
    else {
        c2b_globals.gff->element->end_str[0] = '\0';
        c2b_globals.gff->element->end = 1;
    }

    /* 5 - score */
    ssize_t score_size = gff_field_offsets[5] - gff_field_offsets[4] - 1;
    if (score_size >= c2b_globals.gff->element->score_capacity) {
        char *score_resized = NULL;
        score_resized = realloc(c2b_globals.gff->element->score, score_size + 1);
        if (score_resized) {
            c2b_globals.gff->element->score = score_resized;
            c2b_globals.gff->element->score_capacity = score_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SCORE string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (score_size > 0) {
        memcpy(c2b_globals.gff->element->score, src + gff_field_offsets[4] + 1, score_size);
        c2b_globals.gff->element->score[score_size] = '\0';
    }
    else {
        c2b_globals.gff->element->score[0] = '\0';
    }

    /* 6 - strand */
    ssize_t strand_size = gff_field_offsets[6] - gff_field_offsets[5] - 1;
    if (strand_size >= c2b_globals.gff->element->strand_capacity) {
        char *strand_resized = NULL;
        strand_resized = realloc(c2b_globals.gff->element->strand, strand_size + 1);
        if (strand_resized) {
            c2b_globals.gff->element->strand = strand_resized;
            c2b_globals.gff->element->strand_capacity = strand_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize STRAND string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (strand_size > 0) {
        memcpy(c2b_globals.gff->element->strand, src + gff_field_offsets[5] + 1, strand_size);
        c2b_globals.gff->element->strand[strand_size] = '\0';
    }
    else {
        c2b_globals.gff->element->strand[0] = '\0';
    }

    /* 7 - phase */
    ssize_t phase_size = gff_field_offsets[7] - gff_field_offsets[6] - 1;
    if (phase_size >= c2b_globals.gff->element->phase_capacity) {
        char *phase_resized = NULL;
        phase_resized = realloc(c2b_globals.gff->element->phase, phase_size + 1);
        if (phase_resized) {
            c2b_globals.gff->element->phase = phase_resized;
            c2b_globals.gff->element->phase_capacity = phase_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize PHASE string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (phase_size > 0) {
        memcpy(c2b_globals.gff->element->phase, src + gff_field_offsets[6] + 1, phase_size);
        c2b_globals.gff->element->phase[phase_size] = '\0';
    }
    else {
        c2b_globals.gff->element->phase[0] = '\0';
    }

    /* 8 - attributes */
    ssize_t attributes_size = gff_field_offsets[8] - gff_field_offsets[7] - 1;
    if (attributes_size >= c2b_globals.gff->element->attributes_capacity) {
        char *attributes_resized = NULL;
        attributes_resized = realloc(c2b_globals.gff->element->attributes, attributes_size * 2);
        if (attributes_resized) {
            c2b_globals.gff->element->attributes = attributes_resized;
            c2b_globals.gff->element->attributes_capacity = attributes_size * 2;
        }
        else {
            fprintf(stderr, "Error: Could not resize ATTRIBUTES string in GFF element struct\n");
            exit(ENOMEM);
        }
    }
    if (attributes_size > 0) {
        memcpy(c2b_globals.gff->element->attributes, src + gff_field_offsets[7] + 1, attributes_size);
        c2b_globals.gff->element->attributes[attributes_size] = '\0';
    }
    else {
        c2b_globals.gff->element->attributes[0] = '\0';
    }

    /* 
       Fix coordinate indexing, and (if needed) add attribute for zero-length record
    */

    if (c2b_globals.gff->element->start == c2b_globals.gff->element->end) {
        c2b_globals.gff->element->start -= 1;
        ssize_t trailing_semicolon_fudge = (c2b_globals.gff->element->attributes[strlen(c2b_globals.gff->element->attributes) - 1] == ';') ? 1 : 0;
        ssize_t new_attributes_size_with_zlia = strlen(c2b_globals.gff->element->attributes) + strlen(c2b_gff_zero_length_insertion_attribute);
        if (new_attributes_size_with_zlia >= c2b_globals.gff->element->attributes_capacity) {
            char *attributes_resized_for_zlia = NULL;
            attributes_resized_for_zlia = realloc(c2b_globals.gff->element->attributes, new_attributes_size_with_zlia + 1);
            if (attributes_resized_for_zlia) {
                c2b_globals.gff->element->attributes = attributes_resized_for_zlia;
                c2b_globals.gff->element->attributes_capacity = new_attributes_size_with_zlia + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize ATTRIBUTES string (for ZLIA) in GFF element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.gff->element->attributes + strlen(c2b_globals.gff->element->attributes) - trailing_semicolon_fudge,
               c2b_gff_zero_length_insertion_attribute, 
               strlen(c2b_gff_zero_length_insertion_attribute) + 1);
        c2b_globals.gff->element->attributes[new_attributes_size_with_zlia] = '\0';
    }
    else {
        c2b_globals.gff->element->start -= 1;
    }

    /* 
       Parse ID value out from attributes string
    */

    char *attributes_copy = NULL;
    attributes_copy = malloc(strlen(c2b_globals.gff->element->attributes) + 1);
    if (!attributes_copy) {
        fprintf(stderr, "Error: Could not allocate space for GFF attributes copy\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    memcpy(attributes_copy, c2b_globals.gff->element->attributes, strlen(c2b_globals.gff->element->attributes) + 1);
    const char *kv_tok;
    const char *gff_id_prefix = "ID=";
    const char *gff_null_id = ".";
    char *id_str = NULL;
    memcpy(c2b_globals.gff->element->id, gff_null_id, strlen(gff_null_id) + 1);
    c2b_globals.gff->element->id[strlen(gff_null_id)] = '\0';
    while ((kv_tok = c2b_strsep(&attributes_copy, ";")) != NULL) {
        id_str = strstr(kv_tok, gff_id_prefix);
        if (id_str) {
            ssize_t id_size = strlen(id_str);
            if (id_size >= c2b_globals.gff->element->id_capacity) {
                char *id_resized = NULL;
                id_resized = realloc(c2b_globals.gff->element->id, id_size + 1);
                if (id_resized) {
                    c2b_globals.gff->element->id = id_resized;
                    c2b_globals.gff->element->id_capacity = id_size + 1;
                }
                else {
                    fprintf(stderr, "Error: Could not resize ID string in GFF element struct\n");
                    exit(ENOMEM);
                }
            }
            if (id_size > 0) {
                memcpy(c2b_globals.gff->element->id, kv_tok + strlen(gff_id_prefix), strlen(kv_tok + strlen(gff_id_prefix)) + 1);
                c2b_globals.gff->element->id[strlen(kv_tok + strlen(gff_id_prefix)) + 1] = '\0';
            }
            else {
                c2b_globals.gff->element->id[0] = '\0';
            }
        }
    }
    free(attributes_copy), attributes_copy = NULL;

    /* 
       Convert GFF struct to BED string and copy it to destination
    */

    c2b_line_convert_gff_ptr_to_bed(c2b_globals.gff->element, dest, dest_size, dest_capacity);
}

static inline void
c2b_line_convert_gff_ptr_to_bed(c2b_gff_t* g, char** dest_line_ptr, ssize_t* dest_size, ssize_t* dest_capacity)
{
    /* 
       For GFF- and GVF-formatted data, we use the mapping provided by BEDOPS convention described at:

       http://bedops.readthedocs.io/en/latest/content/reference/file-management/conversion/gff2bed.html

       GFF field                 BED column index       BED field
       -------------------------------------------------------------------------
       seqid                     1                      chromosome
       start                     2                      start
       end                       3                      stop
       ID (via attributes)       4                      id
       score                     5                      score
       strand                    6                      strand

       The remaining GFF columns are mapped as-is, in same order, to adjacent BED columns:

       GFF field                 BED column index       BED field
       -------------------------------------------------------------------------
       source                    7                      -
       type                      8                      -
       phase                     9                      -
       attributes                10                     -
    */

    if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
        char *dest_line_resized = NULL;
        dest_line_resized = realloc(*dest_line_ptr, *dest_capacity * 2);
        if (dest_line_resized) {
            *dest_capacity *= 2;
            *dest_line_ptr = dest_line_resized;
        }
        else {
            fprintf(stderr, "Error: Could not resize dest_line_ptr string in GFF pointer conversion fn\n");
            exit(ENOMEM);
        }
    }
    
    if (strlen(g->seqid) == 0) {
        return;
    }

    *dest_size += sprintf(*dest_line_ptr + *dest_size,
                          "%s\t"                \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%s\t"                \
                          "%s\t"                \
                          "%s\t"                \
                          "%s\t"                \
                          "%s\t"                \
                          "%s\t"                \
                          "%s\n",
                          g->seqid,
                          g->start,
                          g->end,
                          g->id,
                          g->score,
                          g->strand,
                          g->source,
                          g->type,
                          g->phase,
                          g->attributes);
}

static void
c2b_psl_init_element(c2b_psl_t** e)
{
    *e = malloc(sizeof(c2b_psl_t));
    if (!*e) {
        fprintf(stderr, "Error: Could not allocate space for PSL element pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    (*e)->matchesStr = NULL, (*e)->matchesStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->matchesStr)));
    if (!(*e)->matchesStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element matchesStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->matchesStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->misMatchesStr = NULL, (*e)->misMatchesStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->misMatchesStr)));
    if (!(*e)->misMatchesStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element misMatchesStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->misMatchesStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->repMatchesStr = NULL, (*e)->repMatchesStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->repMatchesStr)));
    if (!(*e)->repMatchesStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element repMatchesStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->repMatchesStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->nCountStr = NULL, (*e)->nCountStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->nCountStr)));
    if (!(*e)->nCountStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element nCountStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->nCountStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qNumInsertStr = NULL, (*e)->qNumInsertStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qNumInsertStr)));
    if (!(*e)->qNumInsertStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qNumInsertStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qNumInsertStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qBaseInsertStr = NULL, (*e)->qBaseInsertStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qBaseInsertStr)));
    if (!(*e)->qBaseInsertStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qBaseInsertStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qBaseInsertStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tNumInsertStr = NULL, (*e)->tNumInsertStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tNumInsertStr)));
    if (!(*e)->tNumInsertStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tNumInsertStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tNumInsertStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tBaseInsertStr = NULL, (*e)->tBaseInsertStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tBaseInsertStr)));
    if (!(*e)->tBaseInsertStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tBaseInsertStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tBaseInsertStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->strand = NULL, (*e)->strand = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->strand)));
    if (!(*e)->strand) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element strand malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->strand_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qName = NULL, (*e)->qName = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qName)));
    if (!(*e)->qName) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qName malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qName_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qSizeStr = NULL, (*e)->qSizeStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qSizeStr)));
    if (!(*e)->qSizeStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qSizeStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qSizeStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qStartStr = NULL, (*e)->qStartStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qStartStr)));
    if (!(*e)->qStartStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qStartStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qStartStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qEndStr = NULL, (*e)->qEndStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qEndStr)));
    if (!(*e)->qEndStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qEndStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qEndStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tName = NULL, (*e)->tName = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tName)));
    if (!(*e)->tName) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tName malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tName_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tSizeStr = NULL, (*e)->tSizeStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tSizeStr)));
    if (!(*e)->tSizeStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tSizeStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tSizeStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tStartStr = NULL, (*e)->tStartStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tStartStr)));
    if (!(*e)->tStartStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tStartStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tStartStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tEndStr = NULL, (*e)->tEndStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tEndStr)));
    if (!(*e)->tEndStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tEndStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tEndStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->blockCountStr = NULL, (*e)->blockCountStr = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->blockCountStr)));
    if (!(*e)->blockCountStr) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element blockCountStr malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->blockCountStr_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->blockSizes = NULL, (*e)->blockSizes = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->blockSizes)));
    if (!(*e)->blockSizes) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element blockSizes malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->blockSizes_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qStarts = NULL, (*e)->qStarts = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qStarts)));
    if (!(*e)->qStarts) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element qStarts malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qStarts_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tStarts = NULL, (*e)->tStarts = malloc(C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tStarts)));
    if (!(*e)->tStarts) { 
        fprintf(stderr, "Error: Could not allocate space for PSL element tStarts malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tStarts_capacity = C2B_PSL_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->matches = 0;
    (*e)->misMatches = 0;
    (*e)->repMatches = 0;
    (*e)->nCount = 0;
    (*e)->qNumInsert = 0;
    (*e)->qBaseInsert = 0;
    (*e)->tNumInsert = 0;
    (*e)->tBaseInsert = 0;
    (*e)->qSize = 0;
    (*e)->qStart = 0;
    (*e)->qEnd = 0;
    (*e)->tSize = 0;
    (*e)->tStart = 0;
    (*e)->tEnd = 0;
    (*e)->blockCount = 0;
}

static void
c2b_psl_delete_element(c2b_psl_t* e)
{
    if (e->matchesStr)      { free(e->matchesStr),      e->matchesStr = NULL;      }
    if (e->misMatchesStr)   { free(e->misMatchesStr),   e->misMatchesStr = NULL;   }
    if (e->repMatchesStr)   { free(e->repMatchesStr),   e->repMatchesStr = NULL;   }
    if (e->nCountStr)       { free(e->nCountStr),       e->nCountStr = NULL;       }
    if (e->qNumInsertStr)   { free(e->qNumInsertStr),   e->qNumInsertStr = NULL;   }
    if (e->qBaseInsertStr)  { free(e->qBaseInsertStr),  e->qBaseInsertStr = NULL;  }
    if (e->tNumInsertStr)   { free(e->tNumInsertStr),   e->tNumInsertStr = NULL;   }
    if (e->tBaseInsertStr)  { free(e->tBaseInsertStr),  e->tBaseInsertStr = NULL;  }
    if (e->strand)          { free(e->strand),          e->strand = NULL;          }
    if (e->qName)           { free(e->qName),           e->qName = NULL;           }
    if (e->qSizeStr)        { free(e->qSizeStr),        e->qSizeStr = NULL;        }
    if (e->qStartStr)       { free(e->qStartStr),       e->qStartStr = NULL;       }
    if (e->qEndStr)         { free(e->qEndStr),         e->qEndStr = NULL;         }
    if (e->tName)           { free(e->tName),           e->tName = NULL;           }
    if (e->tSizeStr)        { free(e->tSizeStr),        e->tSizeStr = NULL;        }
    if (e->tStartStr)       { free(e->tStartStr),       e->tStartStr = NULL;       }
    if (e->tEndStr)         { free(e->tEndStr),         e->tEndStr = NULL;         }
    if (e->blockCountStr)   { free(e->blockCountStr),   e->blockCountStr = NULL;   }
    if (e->blockSizes)      { free(e->blockSizes),      e->blockSizes = NULL;      }
    if (e->qStarts)         { free(e->qStarts),         e->qStarts = NULL;         }
    if (e->tStarts)         { free(e->tStarts),         e->tStarts = NULL;         }
    if (e)                  { free(e),                  e = NULL;                  }
}

static void
c2b_line_convert_psl_to_bed_unsorted(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    ssize_t psl_field_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    int psl_field_idx = 0;
    ssize_t current_src_posn = -1;

    while (++current_src_posn < src_size) {
        if ((src[current_src_posn] == c2b_tab_delim) || (src[current_src_posn] == c2b_line_delim)) {
            psl_field_offsets[psl_field_idx++] = current_src_posn;
        }
        if (psl_field_idx >= C2B_MAX_FIELD_COUNT_VALUE) {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", psl_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }
    psl_field_offsets[psl_field_idx] = src_size;
    psl_field_offsets[psl_field_idx + 1] = -1;

    /* 
       If number of fields is not in bounds, we may need to exit early
    */

    if (((psl_field_idx + 1) < c2b_psl_field_min) || ((psl_field_idx + 1) > c2b_psl_field_max)) {
        if ((psl_field_idx == 0) || (psl_field_idx == 17)) {
            if ((c2b_globals.psl->is_headered) && (c2b_globals.keep_header_flag) && (c2b_globals.header_line_idx <= 5)) {
                /* set up temporary header stream buffers */
                if (!c2b_globals.src_line_str) {
                    c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                    if (!c2b_globals.src_line_str) {
                        fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                        exit(ENOMEM);
                    }
                }
                if (!c2b_globals.dest_line_str) {
                    c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                    if (!c2b_globals.dest_line_str) {
                        fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                        exit(ENOMEM);
                    }
                }
                /* copy header line to destination stream buffer */
                memcpy(c2b_globals.src_line_str, src, src_size);
                c2b_globals.src_line_str[src_size] = '\0';
                sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
                memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
                *dest_size += strlen(c2b_globals.dest_line_str);
                c2b_globals.header_line_idx++;
                return;                    
            }
            else if ((c2b_globals.psl->is_headered) && (c2b_globals.header_line_idx <= 5)) {
                c2b_globals.header_line_idx++;
                return;
            }
            else {
                fprintf(stderr, "Error: Possible corrupt input on line %u -- if PSL input is headered, use the --headered option\n", c2b_globals.header_line_idx);
                c2b_print_usage(stderr);
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
        }
        else {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may not match input format\n", psl_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }

    /* 0 - matches */
    ssize_t matchesStr_size = psl_field_offsets[0];
    if (matchesStr_size >= c2b_globals.psl->element->matchesStr_capacity) {
        char *matchesStr_resized = NULL;
        matchesStr_resized = realloc(c2b_globals.psl->element->matchesStr, matchesStr_size + 1);
        if (matchesStr_resized) {
            c2b_globals.psl->element->matchesStr = matchesStr_resized;
            c2b_globals.psl->element->matchesStr_capacity = matchesStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize matchesStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->matchesStr, src, matchesStr_size);
    c2b_globals.psl->element->matchesStr[matchesStr_size] = '\0';
    c2b_globals.psl->element->matches = strtoull(c2b_globals.psl->element->matchesStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "matchesStr: [%s]\n", c2b_globals.psl->element->matchesStr);
#endif

    /* 
       We test if matches is a number or string, as one of the header 
       lines can mimic a genomic element
    */

    if ((c2b_globals.psl->element->matches == 0) && (!isdigit(c2b_globals.psl->element->matchesStr[0]))) {
        if ((c2b_globals.psl->is_headered) && (c2b_globals.keep_header_flag) && (c2b_globals.header_line_idx <= 5)) {
            /* set up temporary header stream buffers */
            if (!c2b_globals.src_line_str) {
                c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                if (!c2b_globals.src_line_str) {
                    fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                    exit(ENOMEM);
                }
            }
            if (!c2b_globals.dest_line_str) {
                c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                if (!c2b_globals.dest_line_str) {
                    fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                    exit(ENOMEM);
                }
            }
            /* copy header line to destination stream buffer */
            memcpy(c2b_globals.src_line_str, src, src_size);
            c2b_globals.src_line_str[src_size] = '\0';
            sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
        }
        return;
    }

    /* 1 - misMatches */
    ssize_t misMatchesStr_size = psl_field_offsets[1] - psl_field_offsets[0] - 1;
    if (misMatchesStr_size >= c2b_globals.psl->element->misMatchesStr_capacity) {
        char *misMatchesStr_resized = NULL;
        misMatchesStr_resized = realloc(c2b_globals.psl->element->misMatchesStr, misMatchesStr_size + 1);
        if (misMatchesStr_resized) {
            c2b_globals.psl->element->misMatchesStr = misMatchesStr_resized;
            c2b_globals.psl->element->misMatchesStr_capacity = misMatchesStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize misMatchesStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->misMatchesStr, src + psl_field_offsets[0] + 1, misMatchesStr_size);
    c2b_globals.psl->element->misMatchesStr[misMatchesStr_size] = '\0';
    c2b_globals.psl->element->misMatches = strtoull(c2b_globals.psl->element->misMatchesStr, NULL, 10);
    
#ifdef DEBUG
    fprintf(stderr, "misMatchesStr: [%s]\n", c2b_globals.psl->element->misMatchesStr);
#endif

    /* 2 - repMatches */
    ssize_t repMatchesStr_size = psl_field_offsets[2] - psl_field_offsets[1] - 1;
    if (repMatchesStr_size >= c2b_globals.psl->element->repMatchesStr_capacity) {
        char *repMatchesStr_resized = NULL;
        repMatchesStr_resized = realloc(c2b_globals.psl->element->repMatchesStr, repMatchesStr_size + 1);
        if (repMatchesStr_resized) {
            c2b_globals.psl->element->repMatchesStr = repMatchesStr_resized;
            c2b_globals.psl->element->repMatchesStr_capacity = repMatchesStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize repMatchesStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->repMatchesStr, src + psl_field_offsets[1] + 1, repMatchesStr_size);
    c2b_globals.psl->element->repMatchesStr[repMatchesStr_size] = '\0';
    c2b_globals.psl->element->repMatches = strtoull(c2b_globals.psl->element->repMatchesStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "repMatchesStr: [%s]\n", c2b_globals.psl->element->repMatchesStr);
#endif

    /* 3 - nCount */
    ssize_t nCountStr_size = psl_field_offsets[3] - psl_field_offsets[2] - 1;
    if (nCountStr_size >= c2b_globals.psl->element->nCountStr_capacity) {
        char *nCountStr_resized = NULL;
        nCountStr_resized = realloc(c2b_globals.psl->element->nCountStr, nCountStr_size + 1);
        if (nCountStr_resized) {
            c2b_globals.psl->element->nCountStr = nCountStr_resized;
            c2b_globals.psl->element->nCountStr_capacity = nCountStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize nCountStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->nCountStr, src + psl_field_offsets[2] + 1, nCountStr_size);
    c2b_globals.psl->element->nCountStr[nCountStr_size] = '\0';
    c2b_globals.psl->element->nCount = strtoull(c2b_globals.psl->element->nCountStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "nCountStr: [%s]\n", c2b_globals.psl->element->nCountStr);
#endif

    /* 4 - qNumInsert */
    ssize_t qNumInsertStr_size = psl_field_offsets[4] - psl_field_offsets[3] - 1;
    if (qNumInsertStr_size >= c2b_globals.psl->element->qNumInsertStr_capacity) {
        char *qNumInsertStr_resized = NULL;
        qNumInsertStr_resized = realloc(c2b_globals.psl->element->qNumInsertStr, qNumInsertStr_size + 1);
        if (qNumInsertStr_resized) {
            c2b_globals.psl->element->qNumInsertStr = qNumInsertStr_resized;
            c2b_globals.psl->element->qNumInsertStr_capacity = qNumInsertStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qNumInsertStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qNumInsertStr, src + psl_field_offsets[3] + 1, qNumInsertStr_size);
    c2b_globals.psl->element->qNumInsertStr[qNumInsertStr_size] = '\0';
    c2b_globals.psl->element->qNumInsert = strtoull(c2b_globals.psl->element->qNumInsertStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "qNumInsertStr: [%s]\n", c2b_globals.psl->element->qNumInsertStr);
#endif

    /* 5 - qBaseInsert */
    ssize_t qBaseInsertStr_size = psl_field_offsets[5] - psl_field_offsets[4] - 1;
    if (qBaseInsertStr_size >= c2b_globals.psl->element->qBaseInsertStr_capacity) {
        char *qBaseInsertStr_resized = NULL;
        qBaseInsertStr_resized = realloc(c2b_globals.psl->element->qBaseInsertStr, qBaseInsertStr_size + 1);
        if (qBaseInsertStr_resized) {
            c2b_globals.psl->element->qBaseInsertStr = qBaseInsertStr_resized;
            c2b_globals.psl->element->qBaseInsertStr_capacity = qBaseInsertStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qBaseInsertStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qBaseInsertStr, src + psl_field_offsets[4] + 1, qBaseInsertStr_size);
    c2b_globals.psl->element->qBaseInsertStr[qBaseInsertStr_size] = '\0';
    c2b_globals.psl->element->qBaseInsert = strtoull(c2b_globals.psl->element->qBaseInsertStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "qBaseInsertStr: [%s]\n", c2b_globals.psl->element->qBaseInsertStr);
#endif

    /* 6 - tNumInsert */
    ssize_t tNumInsertStr_size = psl_field_offsets[6] - psl_field_offsets[5] - 1;
    if (tNumInsertStr_size >= c2b_globals.psl->element->tNumInsertStr_capacity) {
        char *tNumInsertStr_resized = NULL;
        tNumInsertStr_resized = realloc(c2b_globals.psl->element->tNumInsertStr, tNumInsertStr_size + 1);
        if (tNumInsertStr_resized) {
            c2b_globals.psl->element->tNumInsertStr = tNumInsertStr_resized;
            c2b_globals.psl->element->tNumInsertStr_capacity = tNumInsertStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tNumInsertStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tNumInsertStr, src + psl_field_offsets[5] + 1, tNumInsertStr_size);
    c2b_globals.psl->element->tNumInsertStr[tNumInsertStr_size] = '\0';
    c2b_globals.psl->element->tNumInsert = strtoull(c2b_globals.psl->element->tNumInsertStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "tNumInsertStr: [%s]\n", c2b_globals.psl->element->tNumInsertStr);
#endif

    /* 7 - tBaseInsert */
    ssize_t tBaseInsertStr_size = psl_field_offsets[7] - psl_field_offsets[6] - 1;
    if (tBaseInsertStr_size >= c2b_globals.psl->element->tBaseInsertStr_capacity) {
        char *tBaseInsertStr_resized = NULL;
        tBaseInsertStr_resized = realloc(c2b_globals.psl->element->tBaseInsertStr, tBaseInsertStr_size + 1);
        if (tBaseInsertStr_resized) {
            c2b_globals.psl->element->tBaseInsertStr = tBaseInsertStr_resized;
            c2b_globals.psl->element->tBaseInsertStr_capacity = tBaseInsertStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tBaseInsertStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tBaseInsertStr, src + psl_field_offsets[6] + 1, tBaseInsertStr_size);
    c2b_globals.psl->element->tBaseInsertStr[tBaseInsertStr_size] = '\0';
    c2b_globals.psl->element->tBaseInsert = strtoull(c2b_globals.psl->element->tBaseInsertStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "tBaseInsertStr: [%s]\n", c2b_globals.psl->element->tBaseInsertStr);
#endif

    /* 8 - strand */
    ssize_t strand_size = psl_field_offsets[8] - psl_field_offsets[7] - 1;
    if (strand_size >= c2b_globals.psl->element->strand_capacity) {
        char *strand_resized = NULL;
        strand_resized = realloc(c2b_globals.psl->element->strand, strand_size + 1);
        if (strand_resized) {
            c2b_globals.psl->element->strand = strand_resized;
            c2b_globals.psl->element->strand_capacity = strand_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize strand in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->strand, src + psl_field_offsets[7] + 1, strand_size);
    c2b_globals.psl->element->strand[strand_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "strand: [%s]\n", c2b_globals.psl->element->strand);
#endif

    /* 9 - qName */
    ssize_t qName_size = psl_field_offsets[9] - psl_field_offsets[8] - 1;
    if (qName_size >= c2b_globals.psl->element->qName_capacity) {
        char *qName_resized = NULL;
        qName_resized = realloc(c2b_globals.psl->element->qName, qName_size + 1);
        if (qName_resized) {
            c2b_globals.psl->element->qName = qName_resized;
            c2b_globals.psl->element->qName_capacity = qName_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qName in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qName, src + psl_field_offsets[8] + 1, qName_size);
    c2b_globals.psl->element->qName[qName_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "qName: [%s]\n", c2b_globals.psl->element->qName);
#endif

    /* 10 - qSize */
    ssize_t qSizeStr_size = psl_field_offsets[10] - psl_field_offsets[9] - 1;
    if (qSizeStr_size >= c2b_globals.psl->element->qSizeStr_capacity) {
        char *qSizeStr_resized = NULL;
        qSizeStr_resized = realloc(c2b_globals.psl->element->qSizeStr, qSizeStr_size + 1);
        if (qSizeStr_resized) {
            c2b_globals.psl->element->qSizeStr = qSizeStr_resized;
            c2b_globals.psl->element->qSizeStr_capacity = qSizeStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qSizeStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qSizeStr, src + psl_field_offsets[9] + 1, qSizeStr_size);
    c2b_globals.psl->element->qSizeStr[qSizeStr_size] = '\0';
    c2b_globals.psl->element->qSize = strtoull(c2b_globals.psl->element->qSizeStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "qSizeStr: [%s]\n", c2b_globals.psl->element->qSizeStr);
#endif

    /* 11 - qStart */
    ssize_t qStartStr_size = psl_field_offsets[11] - psl_field_offsets[10] - 1;
    if (qStartStr_size >= c2b_globals.psl->element->qStartStr_capacity) {
        char *qStartStr_resized = NULL;
        qStartStr_resized = realloc(c2b_globals.psl->element->qStartStr, qStartStr_size + 1);
        if (qStartStr_resized) {
            c2b_globals.psl->element->qStartStr = qStartStr_resized;
            c2b_globals.psl->element->qStartStr_capacity = qStartStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qStartStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qStartStr, src + psl_field_offsets[10] + 1, qStartStr_size);
    c2b_globals.psl->element->qStartStr[qStartStr_size] = '\0';
    c2b_globals.psl->element->qStart = strtoull(c2b_globals.psl->element->qStartStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "qStartStr: [%s]\n", c2b_globals.psl->element->qStartStr);
#endif

    /* 12 - qEnd */
    ssize_t qEndStr_size = psl_field_offsets[12] - psl_field_offsets[11] - 1;
    if (qEndStr_size >= c2b_globals.psl->element->qEndStr_capacity) {
        char *qEndStr_resized = NULL;
        qEndStr_resized = realloc(c2b_globals.psl->element->qEndStr, qEndStr_size + 1);
        if (qEndStr_resized) {
            c2b_globals.psl->element->qEndStr = qEndStr_resized;
            c2b_globals.psl->element->qEndStr_capacity = qEndStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qEndStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qEndStr, src + psl_field_offsets[11] + 1, qEndStr_size);
    c2b_globals.psl->element->qEndStr[qEndStr_size] = '\0';
    c2b_globals.psl->element->qEnd = strtoull(c2b_globals.psl->element->qEndStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "qEndStr: [%s]\n", c2b_globals.psl->element->qEndStr);
#endif

    /* 13 - tName */
    ssize_t tName_size = psl_field_offsets[13] - psl_field_offsets[12] - 1;
    if (tName_size >= c2b_globals.psl->element->tName_capacity) {
        char *tName_resized = NULL;
        tName_resized = realloc(c2b_globals.psl->element->tName, tName_size + 1);
        if (tName_resized) {
            c2b_globals.psl->element->tName = tName_resized;
            c2b_globals.psl->element->tName_capacity = tName_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tName in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tName, src + psl_field_offsets[12] + 1, tName_size);
    c2b_globals.psl->element->tName[tName_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "tName: [%s]\n", c2b_globals.psl->element->tName);
#endif

    /* 14 - tSize */
    ssize_t tSizeStr_size = psl_field_offsets[14] - psl_field_offsets[13] - 1;
    if (tSizeStr_size >= c2b_globals.psl->element->tSizeStr_capacity) {
        char *tSizeStr_resized = NULL;
        tSizeStr_resized = realloc(c2b_globals.psl->element->tSizeStr, tSizeStr_size + 1);
        if (tSizeStr_resized) {
            c2b_globals.psl->element->tSizeStr = tSizeStr_resized;
            c2b_globals.psl->element->tSizeStr_capacity = tSizeStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tSizeStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tSizeStr, src + psl_field_offsets[13] + 1, tSizeStr_size);
    c2b_globals.psl->element->tSizeStr[tSizeStr_size] = '\0';
    c2b_globals.psl->element->tSize = strtoull(c2b_globals.psl->element->tSizeStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "tSizeStr: [%s]\n", c2b_globals.psl->element->tSizeStr);
#endif

    /* 15 - tStart */
    ssize_t tStartStr_size = psl_field_offsets[15] - psl_field_offsets[14] - 1;
    if (tStartStr_size >= c2b_globals.psl->element->tStartStr_capacity) {
        char *tStartStr_resized = NULL;
        tStartStr_resized = realloc(c2b_globals.psl->element->tStartStr, tStartStr_size + 1);
        if (tStartStr_resized) {
            c2b_globals.psl->element->tStartStr = tStartStr_resized;
            c2b_globals.psl->element->tStartStr_capacity = tStartStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tStartStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tStartStr, src + psl_field_offsets[14] + 1, tStartStr_size);
    c2b_globals.psl->element->tStartStr[tStartStr_size] = '\0';
    c2b_globals.psl->element->tStart = strtoull(c2b_globals.psl->element->tStartStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "tStartStr: [%s]\n", c2b_globals.psl->element->tStartStr);
#endif

    /* 16 - tEnd */
    ssize_t tEndStr_size = psl_field_offsets[16] - psl_field_offsets[15] - 1;
    if (tEndStr_size >= c2b_globals.psl->element->tEndStr_capacity) {
        char *tEndStr_resized = NULL;
        tEndStr_resized = realloc(c2b_globals.psl->element->tEndStr, tEndStr_size + 1);
        if (tEndStr_resized) {
            c2b_globals.psl->element->tEndStr = tEndStr_resized;
            c2b_globals.psl->element->tEndStr_capacity = tEndStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tEndStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tEndStr, src + psl_field_offsets[15] + 1, tEndStr_size);
    c2b_globals.psl->element->tEndStr[tEndStr_size] = '\0';
    c2b_globals.psl->element->tEnd = strtoull(c2b_globals.psl->element->tEndStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "tEndStr: [%s]\n", c2b_globals.psl->element->tEndStr);
#endif

    /* 17 - blockCount */
    ssize_t blockCountStr_size = psl_field_offsets[17] - psl_field_offsets[16] - 1;
    if (blockCountStr_size >= c2b_globals.psl->element->blockCountStr_capacity) {
        char *blockCountStr_resized = NULL;
        blockCountStr_resized = realloc(c2b_globals.psl->element->blockCountStr, blockCountStr_size + 1);
        if (blockCountStr_resized) {
            c2b_globals.psl->element->blockCountStr = blockCountStr_resized;
            c2b_globals.psl->element->blockCountStr_capacity = blockCountStr_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize blockCountStr in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->blockCountStr, src + psl_field_offsets[16] + 1, blockCountStr_size);
    c2b_globals.psl->element->blockCountStr[blockCountStr_size] = '\0';
    c2b_globals.psl->element->blockCount = strtoull(c2b_globals.psl->element->blockCountStr, NULL, 10);

#ifdef DEBUG
    fprintf(stderr, "blockCountStr: [%s]\n", c2b_globals.psl->element->blockCountStr);
#endif

    /* 18 - blockSizes */
    ssize_t blockSizes_size = psl_field_offsets[18] - psl_field_offsets[17] - 1;
    if (blockSizes_size >= c2b_globals.psl->element->blockSizes_capacity) {
        char *blockSizes_resized = NULL;
        blockSizes_resized = realloc(c2b_globals.psl->element->blockSizes, blockSizes_size + 1);
        if (blockSizes_resized) {
            c2b_globals.psl->element->blockSizes = blockSizes_resized;
            c2b_globals.psl->element->blockSizes_capacity = blockSizes_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize blockSizes in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->blockSizes, src + psl_field_offsets[17] + 1, blockSizes_size);
    c2b_globals.psl->element->blockSizes[blockSizes_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "blockSizes: [%s]\n", c2b_globals.psl->element->blockSizes);
#endif

    /* 19 - qStarts */
    ssize_t qStarts_size = psl_field_offsets[19] - psl_field_offsets[18] - 1;
    if (qStarts_size >= c2b_globals.psl->element->qStarts_capacity) {
        char *qStarts_resized = NULL;
        qStarts_resized = realloc(c2b_globals.psl->element->qStarts, qStarts_size + 1);
        if (qStarts_resized) {
            c2b_globals.psl->element->qStarts = qStarts_resized;
            c2b_globals.psl->element->qStarts_capacity = qStarts_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize qStarts in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->qStarts, src + psl_field_offsets[18] + 1, qStarts_size);
    c2b_globals.psl->element->qStarts[qStarts_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "qStarts: [%s]\n", c2b_globals.psl->element->qStarts);
#endif

    /* 20 - tStarts */
    ssize_t tStarts_size = psl_field_offsets[20] - psl_field_offsets[19] - 1;
    if (tStarts_size >= c2b_globals.psl->element->tStarts_capacity) {
        char *tStarts_resized = NULL;
        tStarts_resized = realloc(c2b_globals.psl->element->tStarts, tStarts_size + 1);
        if (tStarts_resized) {
            c2b_globals.psl->element->tStarts = tStarts_resized;
            c2b_globals.psl->element->tStarts_capacity = tStarts_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize tStarts in PSL element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.psl->element->tStarts, src + psl_field_offsets[19] + 1, tStarts_size);
    c2b_globals.psl->element->tStarts[tStarts_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "tStarts: [%s]\n", c2b_globals.psl->element->tStarts);
#endif

    /* 
       Convert PSL struct to BED string and copy it to destination
    */

    if ((c2b_globals.split_flag) && (c2b_globals.psl->element->blockCount > 1)) {
        if (c2b_globals.psl->block->max_count < c2b_globals.psl->element->blockCount) {
            fprintf(stderr, "Error: Insufficent PSL block state global size\n");
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        /* parse tStarts and blockSizes strings to write per-block elements */
        c2b_psl_blockSizes_to_ptr(c2b_globals.psl->element->blockSizes, c2b_globals.psl->element->blockCount);
        c2b_psl_tStarts_to_ptr(c2b_globals.psl->element->tStarts, c2b_globals.psl->element->blockCount);
        for (uint64_t bc_idx = 0; bc_idx < c2b_globals.psl->element->blockCount; bc_idx++) {
            c2b_globals.psl->element->tStart = c2b_globals.psl->block->starts[bc_idx];
            c2b_globals.psl->element->tEnd = c2b_globals.psl->block->starts[bc_idx] + c2b_globals.psl->block->sizes[bc_idx];
            c2b_line_convert_psl_ptr_to_bed(c2b_globals.psl->element, dest, dest_size, dest_capacity);
        }
    }
    else {
        c2b_line_convert_psl_ptr_to_bed(c2b_globals.psl->element, dest, dest_size, dest_capacity);
    }
}

static inline void
c2b_psl_blockSizes_to_ptr(char *s, uint64_t bc) 
{
    size_t start_bs_offset = 0;
    size_t end_bs_index = 0;
    size_t length_bs_offset = 0;
    uint64_t bc_idx;
    char bs_arr[C2B_MAX_PSL_BLOCK_SIZES_STRING_LENGTH];
    char *bs_ptr = NULL;
    uint64_t bs_val = 0;

    for (bc_idx = 0; bc_idx < bc; bc_idx++) {
        bs_ptr = strchr(s + start_bs_offset, c2b_psl_blockSizes_delimiter);
#ifdef DEBUG
        fprintf(stderr, "s: [%s] | bs_ptr: [%s]\n", s, bs_ptr);
#endif
        if (bs_ptr) {
            end_bs_index = bs_ptr - s;
#ifdef DEBUG
            fprintf(stderr, "start_bs_offset: [%zu] | end_bs_index: [%zu]\n", start_bs_offset, end_bs_index);
#endif
            length_bs_offset = end_bs_index - start_bs_offset;
            if (length_bs_offset > C2B_MAX_PSL_BLOCK_SIZES_STRING_LENGTH) {
                fprintf(stderr, "Error: PSL block size string length too long\n");
                exit(EINVAL); // Invalid argument (POSIX.1)
            }
            memcpy(bs_arr, s + start_bs_offset, length_bs_offset);
            bs_arr[length_bs_offset] = '\0';
#ifdef DEBUG
            fprintf(stderr, "bs_arr: [%s]\n", bs_arr);
#endif
            bs_val = strtoull(bs_arr, NULL, 10);
            c2b_globals.psl->block->sizes[bc_idx] = bs_val;
            start_bs_offset = end_bs_index + 1;
        }
    }
}

static inline void
c2b_psl_tStarts_to_ptr(char *s, uint64_t bc) 
{
    size_t start_ts_offset = 0;
    size_t end_ts_index = 0;
    size_t length_ts_offset = 0;
    uint64_t bc_idx;
    char ts_arr[C2B_MAX_PSL_T_STARTS_STRING_LENGTH];
    char *ts_ptr = NULL;
    uint64_t ts_val = 0;

    for (bc_idx = 0; bc_idx < bc; bc_idx++) {
        ts_ptr = strchr(s + start_ts_offset, c2b_psl_tStarts_delimiter);
#ifdef DEBUG
        fprintf(stderr, "s: [%s] | ts_ptr: [%s]\n", s, ts_ptr);
#endif
        if (ts_ptr) {
            end_ts_index = ts_ptr - s;
#ifdef DEBUG
            fprintf(stderr, "start_ts_offset: [%zu] | end_ts_index: [%zu]\n", start_ts_offset, end_ts_index);
#endif
            length_ts_offset = end_ts_index - start_ts_offset;
            if (length_ts_offset > C2B_MAX_PSL_T_STARTS_STRING_LENGTH) {
                fprintf(stderr, "Error: PSL block start string length too long\n");
                exit(EINVAL); // Invalid argument (POSIX.1)
            }
            memcpy(ts_arr, s + start_ts_offset, length_ts_offset);
            ts_arr[length_ts_offset] = '\0';
#ifdef DEBUG
            fprintf(stderr, "ts_arr: [%s]\n", ts_arr);
#endif
            ts_val = strtoull(ts_arr, NULL, 10);
            c2b_globals.psl->block->starts[bc_idx] = ts_val;
            start_ts_offset = end_ts_index + 1;
        }
    }
}

static inline void
c2b_line_convert_psl_ptr_to_bed(c2b_psl_t* p, char** dest_line_ptr, ssize_t* dest_size, ssize_t* dest_capacity)
{
    /* 
       For PSL-formatted data, we use the mapping provided by BEDOPS convention described at:

       http://bedops.readthedocs.io/en/latest/content/reference/file-management/conversion/psl2bed.html

       PSL field                 BED column index       BED field
       -------------------------------------------------------------------------
       tName                     1                      chromosome
       tStart                    2                      start
       tEnd                      3                      stop
       qName                     4                      id
       matches                   5                      score
       strand                    6                      strand

       The remaining PSL columns are mapped as-is, in same order, to adjacent BED columns:

       PSL field                 BED column index       BED field
       -------------------------------------------------------------------------
       qSize                     7                      -
       misMatches                8                      -
       repMatches                9                      -
       nCount                    10                     -
       qNumInsert                11                     -
       qBaseInsert               12                     -
       tNumInsert                13                     -
       tBaseInsert               14                     -
       qStart                    15                     -
       qEnd                      16                     -
       tSize                     17                     -
       blockCount                18                     -
       blockSizes                19                     -
       qStarts                   20                     -
       tStarts                   21                     -
    */

    if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
        char *dest_line_resized = NULL;
        dest_line_resized = realloc(*dest_line_ptr, *dest_capacity * 2);
        if (dest_line_resized) {
            *dest_capacity *= 2;
            *dest_line_ptr = dest_line_resized;
        }
        else {
            fprintf(stderr, "Error: Could not resize dest_line_ptr string in PSL pointer conversion fn\n");
            exit(ENOMEM);
        }
    }

    *dest_size += sprintf(*dest_line_ptr + *dest_size,
                          "%s\t"                \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%s\t"                \
                          "%" PRIu64 "\t"       \
                          "%s\t"                \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%" PRIu64 "\t"       \
                          "%s\t"                \
                          "%s\t"                \
                          "%s\n",
                          p->tName,
                          p->tStart,
                          p->tEnd,
                          p->qName,
                          p->matches,
                          p->strand,
                          p->qSize,
                          p->misMatches,
                          p->repMatches,
                          p->nCount,
                          p->qNumInsert,
                          p->qBaseInsert,
                          p->tNumInsert,
                          p->tBaseInsert,
                          p->qStart,
                          p->qEnd,
                          p->tSize,
                          p->blockCount,
                          p->blockSizes,
                          p->qStarts,
                          p->tStarts);
}

static void
c2b_rmsk_init_element(c2b_rmsk_t** e)
{
    *e = malloc(sizeof(c2b_rmsk_t));
    if (!*e) {
        fprintf(stderr, "Error: Could not allocate space for RMSK element pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    (*e)->query_start = 0;
    (*e)->query_start_str = NULL, (*e)->query_start_str = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->query_start_str)));
    if (!(*e)->query_start_str) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element start string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->query_start_str_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->query_end = 0;
    (*e)->query_end_str = NULL, (*e)->query_end_str = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->query_end_str)));
    if (!(*e)->query_end_str) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element end string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->query_end_str_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->sw_score = NULL, (*e)->sw_score = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->sw_score)));
    if (!(*e)->sw_score) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element sw_score malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->sw_score_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->perc_div = NULL, (*e)->perc_div = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->perc_div)));
    if (!(*e)->perc_div) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element perc_div malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->perc_div_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->perc_deleted = NULL, (*e)->perc_deleted = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->perc_deleted)));
    if (!(*e)->perc_deleted) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element perc_deleted malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->perc_deleted_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->perc_inserted = NULL, (*e)->perc_inserted = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->perc_inserted)));
    if (!(*e)->perc_inserted) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element perc_inserted malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->perc_inserted_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->query_seq = NULL, (*e)->query_seq = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->query_seq)));
    if (!(*e)->query_seq) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element query_seq malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->query_seq_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->bases_past_match = NULL, (*e)->bases_past_match = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->bases_past_match)));
    if (!(*e)->bases_past_match) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element bases_past_match malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->bases_past_match_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->strand = NULL, (*e)->strand = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->strand)));
    if (!(*e)->strand) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element strand malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->strand_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->repeat_name = NULL, (*e)->repeat_name = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->repeat_name)));
    if (!(*e)->repeat_name) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element repeat_name malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->repeat_name_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->repeat_class = NULL, (*e)->repeat_class = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->repeat_class)));
    if (!(*e)->repeat_class) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element repeat_class malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->repeat_class_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->bases_before_match_comp = NULL, (*e)->bases_before_match_comp = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->bases_before_match_comp)));
    if (!(*e)->bases_before_match_comp) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element bases_before_match_comp malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->bases_before_match_comp_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->match_start = NULL, (*e)->match_start = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->match_start)));
    if (!(*e)->match_start) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element match_start malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->match_start_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->match_end = NULL, (*e)->match_end = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->match_end)));
    if (!(*e)->match_end) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element match_end malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->match_end_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->unique_id = NULL, (*e)->unique_id = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->unique_id)));
    if (!(*e)->unique_id) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element unique_id malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->unique_id_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->higher_score_match = NULL, (*e)->higher_score_match = malloc(C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->higher_score_match)));
    if (!(*e)->higher_score_match) { 
        fprintf(stderr, "Error: Could not allocate space for RMSK element higher_score_match malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->higher_score_match_capacity = C2B_RMSK_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;
}

static void
c2b_rmsk_delete_element(c2b_rmsk_t* e)
{
    if (e->query_start_str)              { free(e->query_start_str),              e->query_start_str = NULL;              }
    if (e->query_end_str)                { free(e->query_end_str),                e->query_end_str = NULL;                }
    if (e->sw_score)                     { free(e->sw_score),                     e->sw_score = NULL;                     }
    if (e->perc_div)                     { free(e->perc_div),                     e->perc_div = NULL;                     }
    if (e->perc_deleted)                 { free(e->perc_deleted),                 e->perc_deleted = NULL;                 }
    if (e->perc_inserted)                { free(e->perc_inserted),                e->perc_inserted = NULL;                }
    if (e->query_seq)                    { free(e->query_seq),                    e->query_seq = NULL;                    }
    if (e->bases_past_match)             { free(e->bases_past_match),             e->bases_past_match = NULL;             }
    if (e->strand)                       { free(e->strand),                       e->strand = NULL;                       }
    if (e->repeat_name)                  { free(e->repeat_name),                  e->repeat_name = NULL;                  }
    if (e->repeat_class)                 { free(e->repeat_class),                 e->repeat_class = NULL;                 }
    if (e->bases_before_match_comp)      { free(e->bases_before_match_comp),      e->bases_before_match_comp = NULL;      }
    if (e->match_start)                  { free(e->match_start),                  e->match_start = NULL;                  }
    if (e->match_end)                    { free(e->match_end),                    e->match_end = NULL;                    }
    if (e->unique_id)                    { free(e->unique_id),                    e->unique_id = NULL;                    }
    if (e->higher_score_match)           { free(e->higher_score_match),           e->higher_score_match = NULL;           }
    if (e)                               { free(e),                               e = NULL;                               }
}

static void
c2b_line_convert_rmsk_to_bed_unsorted(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    /* 
       RepeatMasker annotation output is space-delimited and can have multiple spaces. We also need to walk
       past the first lines of the the output data to skip the header.
    */

    ssize_t rmsk_field_start_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    ssize_t rmsk_field_end_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    int rmsk_field_start_idx = 0;
    int rmsk_field_end_idx = 0;
    ssize_t current_src_posn = 0;

    /* skip blank lines */
    if (src_size == 0) {
        c2b_globals.rmsk->line++;
        return;
    }

    while (current_src_posn < src_size) {
        /* within bounds */
        if (((current_src_posn + 1) < src_size) && (c2b_globals.rmsk->line >= c2b_rmsk_header_line_count)) {
            /* skip over any initial spaces */
            while (c2b_globals.rmsk->is_start_of_line) {
                if ((src[current_src_posn] != c2b_space_delim) && (src[current_src_posn] != c2b_line_delim)) {
                    c2b_globals.rmsk->is_start_of_line = kFalse;
                    rmsk_field_start_offsets[rmsk_field_start_idx++] = current_src_posn; /* 0th offset is *start* of actual data */
                    break;
                }
                if (rmsk_field_start_idx >= C2B_MAX_FIELD_COUNT_VALUE) {
                    fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", rmsk_field_start_idx);
                    c2b_print_usage(stderr);
                    exit(EINVAL); // Invalid argument (POSIX.1)                    
                }
                current_src_posn++;
            }
            /* if current position is a space delimiter, we keep reading until there are no more spaces */
            if (src[current_src_posn] == c2b_space_delim) {
                c2b_globals.rmsk->is_start_of_gap = kTrue;
                if (c2b_globals.rmsk->is_start_of_gap) {
                    rmsk_field_end_offsets[rmsk_field_end_idx++] = current_src_posn; /* current offset is end of current field */
                }
                /* walk through gap until next field is found */
                while (c2b_globals.rmsk->is_start_of_gap) {
                    if (src[current_src_posn++] != c2b_space_delim) {
                        c2b_globals.rmsk->is_start_of_gap = kFalse;
                        rmsk_field_start_offsets[rmsk_field_start_idx++] = current_src_posn - 1; /* current offset is start of next field */
                        if (src[current_src_posn] == c2b_line_delim) {
                            rmsk_field_end_offsets[rmsk_field_end_idx++] = current_src_posn;
                        }
                        current_src_posn--;
                        break;
                    }
                }
            }
            /* if current position is a line delimiter, we increment some indices */
            else if (src[current_src_posn] == c2b_line_delim) {
                rmsk_field_end_offsets[rmsk_field_end_idx++] = current_src_posn;
                c2b_globals.rmsk->line++;
                c2b_globals.rmsk->is_start_of_line = kTrue;
                c2b_globals.rmsk->is_start_of_gap = kFalse;
            }
        }
        else {
            if (src[current_src_posn + 1] == c2b_line_delim) {
                rmsk_field_end_offsets[rmsk_field_end_idx++] = current_src_posn + 1;
                c2b_globals.rmsk->line++;
                c2b_globals.rmsk->is_start_of_line = kTrue;
                c2b_globals.rmsk->is_start_of_gap = kFalse;
                if (c2b_globals.rmsk->line <= c2b_rmsk_header_line_count) {
                    if (c2b_globals.keep_header_flag) {
                        /* set up temporary header stream buffers */
                        if (!c2b_globals.src_line_str) {
                            c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                            if (!c2b_globals.src_line_str) {
                                fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                                exit(ENOMEM);
                            }
                        }
                        if (!c2b_globals.dest_line_str) {
                            c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                            if (!c2b_globals.dest_line_str) {
                                fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                                exit(ENOMEM);
                            }
                        }
                        memcpy(c2b_globals.src_line_str, src, src_size);
                        c2b_globals.src_line_str[src_size] = '\0';
                        sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
                        memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
                        *dest_size += strlen(c2b_globals.dest_line_str);
                        c2b_globals.header_line_idx++;
                    }
                    return;
                }
            }
        }
        current_src_posn++;
    }

    c2b_globals.rmsk->is_start_of_line = kTrue;
    c2b_globals.rmsk->is_start_of_gap = kFalse;

#ifdef DEBUG
    fprintf(stderr, "rmsk_field_start_idx: %d\n", (int) rmsk_field_start_idx);
    fprintf(stderr, "rmsk_field_end_idx: %d\n", (int) rmsk_field_end_idx);
#endif

    if ((rmsk_field_start_idx < c2b_rmsk_field_min) || (rmsk_field_end_idx > c2b_rmsk_field_max)) {
        fprintf(stderr, "Error: Invalid field count (%d) -- input file may not match input format\n", rmsk_field_start_idx);
        c2b_print_usage(stderr);
        exit(EINVAL); // Invalid argument (POSIX.1)
    }
    
    /*  0 - Smith-Waterman score of the match */
    ssize_t sw_score_start = rmsk_field_start_offsets[0];
    ssize_t sw_score_end = rmsk_field_end_offsets[0];
    ssize_t sw_score_size = sw_score_end - sw_score_start;
    if (sw_score_size >= c2b_globals.rmsk->element->sw_score_capacity) {
        char *sw_score_resized = NULL;
        sw_score_resized = realloc(c2b_globals.rmsk->element->sw_score, sw_score_size + 1);
        if (sw_score_resized) {
            c2b_globals.rmsk->element->sw_score = sw_score_resized;
            c2b_globals.rmsk->element->sw_score_capacity = sw_score_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SW_SCORE string in RMSK element struct\n");
            exit(ENOMEM);
        }

    }
    memcpy(c2b_globals.rmsk->element->sw_score, src + sw_score_start, sw_score_size);
    c2b_globals.rmsk->element->sw_score[sw_score_size] = '\0';

#ifdef DEBUG
    fprintf(stderr, "sw_score [%s]\n", c2b_globals.rmsk->element->sw_score);
#endif

    /*  1 - Percent, divergence = mismatches / (matches + mismatches) */
    ssize_t perc_div_start = rmsk_field_start_offsets[1];
    ssize_t perc_div_end = rmsk_field_end_offsets[1];
    ssize_t perc_div_size = perc_div_end - perc_div_start;
    if (perc_div_size >= c2b_globals.rmsk->element->perc_div_capacity) {
        char *perc_div_resized = NULL;
        perc_div_resized = realloc(c2b_globals.rmsk->element->perc_div, perc_div_size + 1);
        if (perc_div_resized) {
            c2b_globals.rmsk->element->perc_div = perc_div_resized;
            c2b_globals.rmsk->element->perc_div_capacity = perc_div_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize PERC_DIV string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (perc_div_size > 0) {
        memcpy(c2b_globals.rmsk->element->perc_div, src + perc_div_start, perc_div_size);
        c2b_globals.rmsk->element->perc_div[perc_div_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->perc_div[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "perc_div [%s]\n", c2b_globals.rmsk->element->perc_div);
#endif

    /*  2 - Percent, bases opposite a gap in the query sequence = deleted bp */
    ssize_t perc_deleted_start = rmsk_field_start_offsets[2];
    ssize_t perc_deleted_end = rmsk_field_end_offsets[2];
    ssize_t perc_deleted_size = perc_deleted_end - perc_deleted_start;
    if (perc_deleted_size >= c2b_globals.rmsk->element->perc_deleted_capacity) {
        char *perc_deleted_resized = NULL;
        perc_deleted_resized = realloc(c2b_globals.rmsk->element->perc_deleted, perc_deleted_size + 1);
        if (perc_deleted_resized) {
            c2b_globals.rmsk->element->perc_deleted = perc_deleted_resized;
            c2b_globals.rmsk->element->perc_deleted_capacity = perc_deleted_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize PERC_DELETED string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (perc_deleted_size > 0) {
        memcpy(c2b_globals.rmsk->element->perc_deleted, src + perc_deleted_start, perc_deleted_size);
        c2b_globals.rmsk->element->perc_deleted[perc_deleted_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->perc_deleted[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "perc_deleted [%s]\n", c2b_globals.rmsk->element->perc_deleted);
#endif

    /*  3 - Percent, bases opposite a gap in the repeat consensus = inserted bp */
    ssize_t perc_inserted_start = rmsk_field_start_offsets[3];
    ssize_t perc_inserted_end = rmsk_field_end_offsets[3];
    ssize_t perc_inserted_size = perc_inserted_end - perc_inserted_start;
    if (perc_inserted_size >= c2b_globals.rmsk->element->perc_inserted_capacity) {
        char *perc_inserted_resized = NULL;
        perc_inserted_resized = realloc(c2b_globals.rmsk->element->perc_inserted, perc_inserted_size + 1);
        if (perc_inserted_resized) {
            c2b_globals.rmsk->element->perc_inserted = perc_inserted_resized;
            c2b_globals.rmsk->element->perc_inserted_capacity = perc_inserted_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize PERC_INSERTED string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (perc_inserted_size > 0) {
        memcpy(c2b_globals.rmsk->element->perc_inserted, src + perc_inserted_start, perc_inserted_size);
        c2b_globals.rmsk->element->perc_inserted[perc_inserted_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->perc_inserted[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "perc_inserted [%s]\n", c2b_globals.rmsk->element->perc_inserted);
#endif

    /*  4 - Query sequence */
    ssize_t query_seq_start = rmsk_field_start_offsets[4];
    ssize_t query_seq_end = rmsk_field_end_offsets[4];
    ssize_t query_seq_size = query_seq_end - query_seq_start;
    if (query_seq_size >= c2b_globals.rmsk->element->query_seq_capacity) {
        char *query_seq_resized = NULL;
        query_seq_resized = realloc(c2b_globals.rmsk->element->query_seq, query_seq_size + 1);
        if (query_seq_resized) {
            c2b_globals.rmsk->element->query_seq = query_seq_resized;
            c2b_globals.rmsk->element->query_seq_capacity = query_seq_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize QUERY_SEQ string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (query_seq_size > 0) {
        memcpy(c2b_globals.rmsk->element->query_seq, src + query_seq_start, query_seq_size);
        c2b_globals.rmsk->element->query_seq[query_seq_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->query_seq[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "query_seq [%s]\n", c2b_globals.rmsk->element->query_seq);
#endif

    /*  5 - Query start (1-indexed) */
    ssize_t query_start_start = rmsk_field_start_offsets[5];
    ssize_t query_start_end = rmsk_field_end_offsets[5];
    ssize_t query_start_size = query_start_end - query_start_start;
    if (query_start_size >= c2b_globals.rmsk->element->query_start_str_capacity) {
        char *query_start_str_resized = NULL;
        query_start_str_resized = realloc(c2b_globals.rmsk->element->query_start_str, query_start_size + 1);
        if (query_start_str_resized) {
            c2b_globals.rmsk->element->query_start_str = query_start_str_resized;
            c2b_globals.rmsk->element->query_start_str_capacity = query_start_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize start string buffer in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if ((query_start_size > 0) && (query_start_size < C2B_MAX_OPERATION_FIELD_LENGTH_VALUE)) {
        memcpy(c2b_globals.rmsk->element->query_start_str, src + query_start_start, query_start_size);
        c2b_globals.rmsk->element->query_start_str[query_start_size] = '\0';
        c2b_globals.rmsk->element->query_start = strtoull(c2b_globals.rmsk->element->query_start_str, NULL, 10);
        if (errno == ERANGE) {
            fprintf(stderr, "Error: Could not convert QUERY_START string [%s] to integer (check input)\n", c2b_globals.rmsk->element->query_start_str);
            exit(ERANGE);
        }
        /* subtract to make 0-indexed */
        if (c2b_globals.rmsk->element->query_start > 0) {
          c2b_globals.rmsk->element->query_start--;
        }
    }
    else {
        c2b_globals.rmsk->element->query_start_str[0] = '\0';
        c2b_globals.rmsk->element->query_start = 0;
    }

#ifdef DEBUG
    fprintf(stderr, "c2b_globals.rmsk->element->query_start [%llu]\n", c2b_globals.rmsk->element->query_start);
    fprintf(stderr, "query_start_str [%s]\n", c2b_globals.rmsk->element->query_start_str);
#endif

    /*  6 - Query end */
    ssize_t query_end_start = rmsk_field_start_offsets[6];
    ssize_t query_end_end = rmsk_field_end_offsets[6];
    ssize_t query_end_size = query_end_end - query_end_start;
    if (query_end_size >= c2b_globals.rmsk->element->query_end_str_capacity) {
        char *query_end_str_resized = NULL;
        query_end_str_resized = realloc(c2b_globals.rmsk->element->query_end_str, query_end_size + 1);
        if (query_end_str_resized) {
            c2b_globals.rmsk->element->query_end_str = query_end_str_resized;
            c2b_globals.rmsk->element->query_end_str_capacity = query_end_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize end string buffer in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if ((query_end_size > 0) && (query_end_size < C2B_MAX_OPERATION_FIELD_LENGTH_VALUE)) {
        memcpy(c2b_globals.rmsk->element->query_end_str, src + query_end_start, query_end_size);
        c2b_globals.rmsk->element->query_end_str[query_end_size] = '\0';
        c2b_globals.rmsk->element->query_end = strtoull(c2b_globals.rmsk->element->query_end_str, NULL, 10);
        if (errno == ERANGE) {
            fprintf(stderr, "Error: Could not convert QUERY_END string [%s] to integer (check input)\n", c2b_globals.rmsk->element->query_end_str);
            exit(ERANGE);
        }      
    }
    else {
        c2b_globals.rmsk->element->query_end_str[0] = '\0';
        c2b_globals.rmsk->element->query_end = 0;
    }

#ifdef DEBUG
    fprintf(stderr, "c2b_globals.rmsk->element->query_end [%llu]\n", c2b_globals.rmsk->element->query_end);
    fprintf(stderr, "query_end_str [%s]\n", c2b_globals.rmsk->element->query_end_str);
#endif

    if (c2b_globals.rmsk->element->query_start == c2b_globals.rmsk->element->query_end) {
        fprintf(stderr, "Error: Invalid query coordinates at line [%llu]\n", c2b_globals.rmsk->line);
        exit(EINVAL);
    }

    /*  7 - Bases in query sequence past the ending position of match */
    ssize_t bases_past_match_start = rmsk_field_start_offsets[7];
    ssize_t bases_past_match_end = rmsk_field_end_offsets[7];
    ssize_t bases_past_match_size = bases_past_match_end - bases_past_match_start;
    if (bases_past_match_size >= c2b_globals.rmsk->element->bases_past_match_capacity) {
        char *bases_past_match_resized = NULL;
        bases_past_match_resized = realloc(c2b_globals.rmsk->element->bases_past_match, bases_past_match_size + 1);
        if (bases_past_match_resized) {
            c2b_globals.rmsk->element->bases_past_match = bases_past_match_resized;
            c2b_globals.rmsk->element->bases_past_match_capacity = bases_past_match_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize BASES_PAST_MATCH string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (bases_past_match_size > 0) {
        memcpy(c2b_globals.rmsk->element->bases_past_match, src + bases_past_match_start, bases_past_match_size);
        c2b_globals.rmsk->element->bases_past_match[bases_past_match_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->bases_past_match[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "bases_past_match [%s]\n", c2b_globals.rmsk->element->bases_past_match);
#endif

    /*  8 - Strand match with repeat consensus sequence (+ = forward, C = complement) */
    ssize_t strand_start = rmsk_field_start_offsets[8];
    ssize_t strand_end = rmsk_field_end_offsets[8];
    ssize_t strand_size = strand_end - strand_start;
    if (strand_size >= c2b_globals.rmsk->element->strand_capacity) {
        char *strand_resized = NULL;
        strand_resized = realloc(c2b_globals.rmsk->element->strand, strand_size + 1);
        if (strand_resized) {
            c2b_globals.rmsk->element->strand = strand_resized;
            c2b_globals.rmsk->element->strand_capacity = strand_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize STRAND string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (strand_size > 0) {
        memcpy(c2b_globals.rmsk->element->strand, src + strand_start, strand_size);
        c2b_globals.rmsk->element->strand[strand_size] = '\0';
        if (strcmp(c2b_globals.rmsk->element->strand, c2b_rmsk_strand_complement) == 0) {
            memcpy(c2b_globals.rmsk->element->strand, c2b_rmsk_strand_complement_replacement, strlen(c2b_rmsk_strand_complement_replacement) + 1);
        }
    }
    else {
        c2b_globals.rmsk->element->strand[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "strand [%s]\n", c2b_globals.rmsk->element->strand);
#endif

    /*  9 - Matching interspersed repeat name */
    ssize_t repeat_name_start = rmsk_field_start_offsets[9];
    ssize_t repeat_name_end = rmsk_field_end_offsets[9];
    ssize_t repeat_name_size = repeat_name_end - repeat_name_start;
    if (repeat_name_size >= c2b_globals.rmsk->element->repeat_name_capacity) {
        char *repeat_name_resized = NULL;
        repeat_name_resized = realloc(c2b_globals.rmsk->element->repeat_name, repeat_name_size + 1);
        if (repeat_name_resized) {
            c2b_globals.rmsk->element->repeat_name = repeat_name_resized;
            c2b_globals.rmsk->element->repeat_name_capacity = repeat_name_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize REPEAT_NAME string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (repeat_name_size > 0) {
        memcpy(c2b_globals.rmsk->element->repeat_name, src + repeat_name_start, repeat_name_size);
        c2b_globals.rmsk->element->repeat_name[repeat_name_size] = '\0'; 
    }
    else {
        c2b_globals.rmsk->element->repeat_name[0] = '\0'; 
    }

#ifdef DEBUG
    fprintf(stderr, "repeat_name [%s]\n", c2b_globals.rmsk->element->repeat_name);
#endif

    /* 10 - Repeat class */
    ssize_t repeat_class_start = rmsk_field_start_offsets[10];
    ssize_t repeat_class_end = rmsk_field_end_offsets[10];
    ssize_t repeat_class_size = repeat_class_end - repeat_class_start;
    if (repeat_class_size >= c2b_globals.rmsk->element->repeat_class_capacity) {
        char *repeat_class_resized = NULL;
        repeat_class_resized = realloc(c2b_globals.rmsk->element->repeat_class, repeat_class_size + 1);
        if (repeat_class_resized) {
            c2b_globals.rmsk->element->repeat_class = repeat_class_resized;
            c2b_globals.rmsk->element->repeat_class_capacity = repeat_class_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize REPEAT_CLASS string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (repeat_class_size > 0) {
        memcpy(c2b_globals.rmsk->element->repeat_class, src + repeat_class_start, repeat_class_size);
        c2b_globals.rmsk->element->repeat_class[repeat_class_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->repeat_class[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "repeat_class [%s]\n", c2b_globals.rmsk->element->repeat_class);
#endif

    /* 11 - Bases in (complement of) the repeat consensus sequence, prior to beginning of the match */
    ssize_t bases_before_match_comp_start = rmsk_field_start_offsets[11];
    ssize_t bases_before_match_comp_end = rmsk_field_end_offsets[11];
    ssize_t bases_before_match_comp_size = bases_before_match_comp_end - bases_before_match_comp_start;
    if (bases_before_match_comp_size >= c2b_globals.rmsk->element->bases_before_match_comp_capacity) {
        char *bases_before_match_comp_resized = NULL;
        bases_before_match_comp_resized = realloc(c2b_globals.rmsk->element->bases_before_match_comp, bases_before_match_comp_size + 1);
        if (bases_before_match_comp_resized) {
            c2b_globals.rmsk->element->bases_before_match_comp = bases_before_match_comp_resized;
            c2b_globals.rmsk->element->bases_before_match_comp_capacity = bases_before_match_comp_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize BASES_BEFORE_MATCH_COMP string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (bases_before_match_comp_size > 0) {
        memcpy(c2b_globals.rmsk->element->bases_before_match_comp, src + bases_before_match_comp_start, bases_before_match_comp_size);
        c2b_globals.rmsk->element->bases_before_match_comp[bases_before_match_comp_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->bases_before_match_comp[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "bases_before_match_comp [%s]\n", c2b_globals.rmsk->element->bases_before_match_comp);
#endif

    /* 12 - Match start (in repeat consensus sequence) */
    ssize_t match_start_start = rmsk_field_start_offsets[12];
    ssize_t match_start_end = rmsk_field_end_offsets[12];
    ssize_t match_start_size = match_start_end - match_start_start;
    if (match_start_size >= c2b_globals.rmsk->element->match_start_capacity) {
        char *match_start_resized = NULL;
        match_start_resized = realloc(c2b_globals.rmsk->element->match_start, match_start_size + 1);
        if (match_start_resized) {
            c2b_globals.rmsk->element->match_start = match_start_resized;
            c2b_globals.rmsk->element->match_start_capacity = match_start_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize MATCH_START string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (match_start_size > 0) {
        memcpy(c2b_globals.rmsk->element->match_start, src + match_start_start, match_start_size);
        c2b_globals.rmsk->element->match_start[match_start_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->match_start[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "match_start [%s]\n", c2b_globals.rmsk->element->match_start);
#endif

    /* 13 - Match end (in repeat consensus sequence) */
    ssize_t match_end_start = rmsk_field_start_offsets[13];
    ssize_t match_end_end = rmsk_field_end_offsets[13];
    ssize_t match_end_size = match_end_end - match_end_start;
    if (match_end_size >= c2b_globals.rmsk->element->match_end_capacity) {
        char *match_end_resized = NULL;
        match_end_resized = realloc(c2b_globals.rmsk->element->match_end, match_end_size + 1);
        if (match_end_resized) {
            c2b_globals.rmsk->element->match_end = match_end_resized;
            c2b_globals.rmsk->element->match_end_capacity = match_end_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize MATCH_END string in RMSK element struct\n");
            exit(ENOMEM);
        }

    }
    if (match_end_size > 0) {
        memcpy(c2b_globals.rmsk->element->match_end, src + match_end_start, match_end_size);
        c2b_globals.rmsk->element->match_end[match_end_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->match_end[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "match_end [%s]\n", c2b_globals.rmsk->element->match_end);
#endif

    /* 14 - Identifier for individual insertions */
    ssize_t unique_id_start = rmsk_field_start_offsets[14];
    ssize_t unique_id_end = rmsk_field_end_offsets[14];
    ssize_t unique_id_size = unique_id_end - unique_id_start;
#ifdef DEBUG
    fprintf(stderr, "unique_id_start [%zd]\n", unique_id_start);
    fprintf(stderr, "unique_id_end [%zd]\n", unique_id_end);
    fprintf(stderr, "unique_id_size [%zd]\n", unique_id_size);
    fprintf(stderr, "c2b_globals.rmsk->element->unique_id_capacity [%zd]\n", c2b_globals.rmsk->element->unique_id_capacity);
#endif
    if (unique_id_size >= c2b_globals.rmsk->element->unique_id_capacity) {
        char *unique_id_resized = NULL;
        unique_id_resized = realloc(c2b_globals.rmsk->element->unique_id, unique_id_size + 1);
        if (unique_id_resized) {
            c2b_globals.rmsk->element->unique_id = unique_id_resized;
            c2b_globals.rmsk->element->unique_id_capacity = unique_id_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize UNIQUE_ID string in RMSK element struct\n");
            exit(ENOMEM);
        }
    }
    if (unique_id_size > 0) {
        memcpy(c2b_globals.rmsk->element->unique_id, src + unique_id_start, unique_id_size);
        c2b_globals.rmsk->element->unique_id[unique_id_size] = '\0';
    }
    else {
        c2b_globals.rmsk->element->unique_id[0] = '\0';
    }

#ifdef DEBUG
    fprintf(stderr, "unique_id [%s]\n", c2b_globals.rmsk->element->unique_id);
#endif

    /* 15 - Higher-scoring match present (optional) */
    if ((rmsk_field_start_idx == c2b_rmsk_field_max) && (rmsk_field_end_idx == c2b_rmsk_field_max)) {
        ssize_t higher_score_match_start = rmsk_field_start_offsets[15];
        ssize_t higher_score_match_end = rmsk_field_end_offsets[15];
        ssize_t higher_score_match_size = higher_score_match_end - higher_score_match_start;
        if (higher_score_match_size >= c2b_globals.rmsk->element->higher_score_match_capacity) {
            char *higher_score_match_resized = NULL;
            higher_score_match_resized = realloc(c2b_globals.rmsk->element->higher_score_match, higher_score_match_size + 1);
            if (higher_score_match_resized) {
                c2b_globals.rmsk->element->higher_score_match = higher_score_match_resized;
                c2b_globals.rmsk->element->higher_score_match_capacity = higher_score_match_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize HIGHER SCORE MATCH string in RMSK element struct\n");
                exit(ENOMEM);
            }
        }
        if (higher_score_match_size > 0) {
            memcpy(c2b_globals.rmsk->element->higher_score_match, src + higher_score_match_start, higher_score_match_size);
            c2b_globals.rmsk->element->higher_score_match[higher_score_match_size] = '\0';
        }
        else {
            c2b_globals.rmsk->element->higher_score_match[0] = '\0';
        }
    }
    
#ifdef DEBUG
    fprintf(stderr, "higher_score_match [%s]\n", c2b_globals.rmsk->element->higher_score_match);
#endif

    c2b_line_convert_rmsk_ptr_to_bed(c2b_globals.rmsk->element, dest, dest_size, dest_capacity);

    /* after writing a line, reset length of element higher_score_match string */
    c2b_globals.rmsk->element->higher_score_match[0] = '\0';
}

static inline void
c2b_line_convert_rmsk_ptr_to_bed(c2b_rmsk_t* r, char** dest_line_ptr, ssize_t* dest_size, ssize_t* dest_capacity)
{
    /* 
       For RepeatMasker annotation-formatted data, we use the mapping provided by BEDOPS
       convention described at:

       http://bedops.readthedocs.io/en/latest/content/reference/file-management/conversion/rmsk2bed.html

       RepeatMasker field        BED column index       BED field
       -------------------------------------------------------------------------
       query_seq                 1                      chromosome
       query_start - 1           2                      start
       query_end                 3                      stop
       repeat_name               4                      id
       sw_score                  5                      score
       strand                    6                      strand

       The remaining RepeatMasker columns are mapped as-is, in same order, to adjacent BED columns:

       RepeatMasker field        BED column index       BED field
       -------------------------------------------------------------------------
       perc_div                  7                      -
       perc_deleted              8                      -
       perc_inserted             9                      -
       bases_past_match          10                     -
       repeat_class              11                     -
       bases_before_match_comp   12                     -
       match_start               13                     -
       match_end                 14                     -
       unique_id                 15                     -
       higher_score_match        16                     -       
    */

    if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
        char *dest_line_resized = NULL;
        dest_line_resized = realloc(*dest_line_ptr, *dest_capacity * 2);
        if (dest_line_resized) {
            *dest_capacity *= 2;
            *dest_line_ptr = dest_line_resized;
        }
        else {
            fprintf(stderr, "Error: Could not resize dest_line_ptr string in RMSK pointer conversion fn\n");
            exit(ENOMEM);
        }
    }

    if (strlen(r->higher_score_match) == 0) {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              r->query_seq,
                              r->query_start,
                              r->query_end,
                              r->repeat_name,
                              r->sw_score,
                              r->strand,
                              r->perc_div,
                              r->perc_deleted,
                              r->perc_inserted,
                              r->bases_past_match,
                              r->repeat_class,
                              r->bases_before_match_comp,
                              r->match_start,
                              r->match_end,
                              r->unique_id);
    }
    else {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              r->query_seq,
                              r->query_start,
                              r->query_end,
                              r->repeat_name,
                              r->sw_score,
                              r->strand,
                              r->perc_div,
                              r->perc_deleted,
                              r->perc_inserted,
                              r->bases_past_match,
                              r->repeat_class,
                              r->bases_before_match_comp,
                              r->match_start,
                              r->match_end,
                              r->unique_id,
                              r->higher_score_match);
    }
}

static void
c2b_line_convert_sam_to_bed_unsorted_without_split_operation(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    /* 
       This functor builds a list of tab delimiters, but first reads in the CIGAR string (6th field) and
       parses it for operation key-value pairs to loop through later on
    */

    ssize_t sam_field_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    int sam_field_idx = 0;
    ssize_t current_src_posn = -1;

    /* 
       Find offsets or process header line 
    */

    if (src[0] == c2b_sam_header_prefix) {
        if (!c2b_globals.keep_header_flag) {
            /* skip header line */
            return;
        }
        else {
            /* set up temporary header stream buffers */
            if (!c2b_globals.src_line_str) {
                c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                if (!c2b_globals.src_line_str) {
                    fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                    exit(ENOMEM);
                }
            }
            if (!c2b_globals.dest_line_str) {
                c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                if (!c2b_globals.dest_line_str) {
                    fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                    exit(ENOMEM);
                }
            }
            /* copy header line to destination stream buffer */
            memcpy(c2b_globals.src_line_str, src, src_size);
            c2b_globals.src_line_str[src_size] = '\0';
            sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
            return;
        }
    }

    while (++current_src_posn < src_size) {
        if ((src[current_src_posn] == c2b_tab_delim) || (src[current_src_posn] == c2b_line_delim)) {
            sam_field_offsets[sam_field_idx++] = current_src_posn;
        }
        if (sam_field_idx >= C2B_MAX_FIELD_COUNT_VALUE) {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", sam_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }
    sam_field_offsets[sam_field_idx] = src_size;
    sam_field_offsets[sam_field_idx + 1] = -1;

    /* 
       If no more than one field is read in, then something went wrong
    */

    if (sam_field_idx == 0) {
        fprintf(stderr, "Error: Invalid field count (%d) -- input file may not match input format\n", sam_field_idx);
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

    /* 
       Translate CIGAR string to operations
    */

    ssize_t cigar_size = sam_field_offsets[5] - sam_field_offsets[4];
    if (cigar_size >= c2b_globals.sam->element->cigar_capacity) {
        char *cigar_resized = NULL;
        cigar_resized = realloc(c2b_globals.sam->element->cigar, cigar_size + 1);
        if (cigar_resized) {
            c2b_globals.sam->element->cigar = cigar_resized;
            c2b_globals.sam->element->cigar_capacity = cigar_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize CIGAR string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->cigar, src + sam_field_offsets[4] + 1, cigar_size - 1);
    c2b_globals.sam->element->cigar[cigar_size - 1] = '\0';
    c2b_sam_cigar_str_to_ops(c2b_globals.sam->element->cigar);
#ifdef DEBUG
    c2b_sam_debug_cigar_ops(c2b_globals.sam->cigar);
#endif
    ssize_t cigar_length = 0;
    ssize_t op_idx = 0;
    for (op_idx = 0; op_idx < c2b_globals.sam->cigar->length; ++op_idx) {
        cigar_length += c2b_globals.sam->cigar->ops[op_idx].bases;
    }

    /* 
       Firstly, is the read mapped? If not, and c2b_globals.all_reads_flag is kFalse, we skip over this line
    */

    ssize_t flag_size = sam_field_offsets[1] - sam_field_offsets[0];
    if (flag_size >= c2b_globals.sam->element->flag_str_capacity) {
        char *flag_str_resized = NULL;
        flag_str_resized = realloc(c2b_globals.sam->element->flag_str, flag_size + 1);
        if (flag_str_resized) {
            c2b_globals.sam->element->flag_str = flag_str_resized;
            c2b_globals.sam->element->flag_str_capacity = flag_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize flag string buffer in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->flag_str, src + sam_field_offsets[0] + 1, flag_size);
    c2b_globals.sam->element->flag_str[flag_size] = '\0';
    c2b_globals.sam->element->flag = (int) strtol(c2b_globals.sam->element->flag_str, NULL, 10);
    boolean is_mapped = (boolean) !(4 & c2b_globals.sam->element->flag);
    if ((!is_mapped) && (!c2b_globals.all_reads_flag)) 
        return;    

    /* 
       Secondly, we need to retrieve RNAME, POS, QNAME parameters
    */

    /* RNAME */
    if (is_mapped) {
        ssize_t rname_size = sam_field_offsets[2] - sam_field_offsets[1] - 1;
        if (rname_size >= c2b_globals.sam->element->rname_capacity) {
            char *rname_resized = NULL;
            rname_resized = realloc(c2b_globals.sam->element->rname, rname_size + 1);
            if (rname_resized) {
                c2b_globals.sam->element->rname = rname_resized;
                c2b_globals.sam->element->rname_capacity = rname_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize RNAME string in SAM element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.sam->element->rname, src + sam_field_offsets[1] + 1, rname_size);
        c2b_globals.sam->element->rname[rname_size] = '\0';
    }
    else {
        ssize_t c2b_unmapped_read_chr_name_size = strlen(c2b_unmapped_read_chr_name);
        if (c2b_unmapped_read_chr_name_size >= c2b_globals.sam->element->rname_capacity) {
            char *rname_resized = NULL;
            rname_resized = realloc(c2b_globals.sam->element->rname, c2b_unmapped_read_chr_name_size + 2);
            if (rname_resized) {
                c2b_globals.sam->element->rname = rname_resized;
                c2b_globals.sam->element->rname_capacity = c2b_unmapped_read_chr_name_size + 2;
            }
            else {
                fprintf(stderr, "Error: Could not resize RNAME string in SAM element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.sam->element->rname, c2b_unmapped_read_chr_name, c2b_unmapped_read_chr_name_size);
        c2b_globals.sam->element->rname[c2b_unmapped_read_chr_name_size] = '\t';
        c2b_globals.sam->element->rname[c2b_unmapped_read_chr_name_size + 1] = '\0';
    }

    /* POS */
    ssize_t pos_size = sam_field_offsets[3] - sam_field_offsets[2];
    if (pos_size >= c2b_globals.sam->element->pos_str_capacity) {
        char *pos_str_resized = NULL;
        pos_str_resized = realloc(c2b_globals.sam->element->pos_str, pos_size + 1);
        if (pos_str_resized) {
            c2b_globals.sam->element->pos_str = pos_str_resized;
            c2b_globals.sam->element->pos_str_capacity = pos_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize position string buffer in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->pos_str, src + sam_field_offsets[2] + 1, pos_size);
    c2b_globals.sam->element->pos_str[pos_size] = '\0';
    uint64_t pos_val = strtoull(c2b_globals.sam->element->pos_str, NULL, 10);

    uint64_t start_val = pos_val - 1; /* remember, start = POS - 1 */
    c2b_globals.sam->element->start = start_val;
    c2b_globals.sam->element->stop = start_val; /* this will be adjusted after CIGAR string is parsed */

    /* QNAME */
    ssize_t qname_size = sam_field_offsets[0];
    if (qname_size >= c2b_globals.sam->element->qname_capacity) {
        char *qname_resized = NULL;
        qname_resized = realloc(c2b_globals.sam->element->qname, qname_size + 1);
        if (qname_resized) {
            c2b_globals.sam->element->qname = qname_resized;
            c2b_globals.sam->element->qname_capacity = qname_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize QNAME string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->qname, src, qname_size);
    c2b_globals.sam->element->qname[qname_size] = '\0';

    /* 16 & FLAG */
    int strand_val = 0x10 & c2b_globals.sam->element->flag;
    char strand_str[C2B_MAX_STRAND_LENGTH_VALUE] = {0};
    sprintf(strand_str, "%c", (strand_val == 0x10) ? '-' : '+');
    ssize_t strand_size = strlen(strand_str);
    if (strand_size >= c2b_globals.sam->element->strand_capacity) {
        char *strand_resized = NULL;
        strand_resized = realloc(c2b_globals.sam->element->strand, strand_size + 1);
        if (strand_resized) {
            c2b_globals.sam->element->strand = strand_resized;
            c2b_globals.sam->element->strand_capacity = strand_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize STRAND string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->strand, strand_str, strand_size);
    c2b_globals.sam->element->strand[strand_size] = '\0';

    /* MAPQ */
    ssize_t mapq_size = sam_field_offsets[4] - sam_field_offsets[3] - 1;
    if (mapq_size >= c2b_globals.sam->element->mapq_capacity) {
        char *mapq_resized = NULL;
        mapq_resized = realloc(c2b_globals.sam->element->mapq, mapq_size + 1);
        if (mapq_resized) {
            c2b_globals.sam->element->mapq = mapq_resized;
            c2b_globals.sam->element->mapq_capacity = mapq_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize MAPQ string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->mapq, src + sam_field_offsets[3] + 1, mapq_size);
    c2b_globals.sam->element->mapq[mapq_size] = '\0';

    /* RNEXT */
    ssize_t rnext_size = sam_field_offsets[6] - sam_field_offsets[5] - 1;
    if (rnext_size >= c2b_globals.sam->element->rnext_capacity) {
        char *rnext_resized = NULL;
        rnext_resized = realloc(c2b_globals.sam->element->rnext, rnext_size + 1);
        if (rnext_resized) {
            c2b_globals.sam->element->rnext = rnext_resized;
            c2b_globals.sam->element->rnext_capacity = rnext_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize RNEXT string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->rnext, src + sam_field_offsets[5] + 1, rnext_size);
    c2b_globals.sam->element->rnext[rnext_size] = '\0';

    /* PNEXT */
    ssize_t pnext_size = sam_field_offsets[7] - sam_field_offsets[6] - 1;
    if (pnext_size >= c2b_globals.sam->element->pnext_capacity) {
        char *pnext_resized = NULL;
        pnext_resized = realloc(c2b_globals.sam->element->pnext, pnext_size + 1);
        if (pnext_resized) {
            c2b_globals.sam->element->pnext = pnext_resized;
            c2b_globals.sam->element->pnext_capacity = pnext_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize PNEXT string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->pnext, src + sam_field_offsets[6] + 1, pnext_size);
    c2b_globals.sam->element->pnext[pnext_size] = '\0';

    /* TLEN */
    ssize_t tlen_size = sam_field_offsets[8] - sam_field_offsets[7] - 1;
    if (tlen_size >= c2b_globals.sam->element->tlen_capacity) {
        char *tlen_resized = NULL;
        tlen_resized = realloc(c2b_globals.sam->element->tlen, tlen_size + 1);
        if (tlen_resized) {
            c2b_globals.sam->element->tlen = tlen_resized;
            c2b_globals.sam->element->tlen_capacity = tlen_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize TLEN string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->tlen, src + sam_field_offsets[7] + 1, tlen_size);
    c2b_globals.sam->element->tlen[tlen_size] = '\0';

    /* SEQ */
    ssize_t seq_size = sam_field_offsets[9] - sam_field_offsets[8] - 1;
    if (seq_size >= c2b_globals.sam->element->seq_capacity) {
        char *seq_resized = NULL;
        seq_resized = realloc(c2b_globals.sam->element->seq, seq_size + 1);
        if (seq_resized) {
            c2b_globals.sam->element->seq = seq_resized;
            c2b_globals.sam->element->seq_capacity = seq_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SEQ string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->seq, src + sam_field_offsets[8] + 1, seq_size);
    c2b_globals.sam->element->seq[seq_size] = '\0';

    /* QUAL */
    ssize_t qual_size = sam_field_offsets[10] - sam_field_offsets[9] - 1;
    if (qual_size >= c2b_globals.sam->element->qual_capacity) {
        char *qual_resized = NULL;
        qual_resized = realloc(c2b_globals.sam->element->qual, qual_size + 1);
        if (qual_resized) {
            c2b_globals.sam->element->qual = qual_resized;
            c2b_globals.sam->element->qual_capacity = qual_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize QUAL string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->qual, src + sam_field_offsets[9] + 1, qual_size);
    c2b_globals.sam->element->qual[qual_size] = '\0';

    /* Optional fields */
    c2b_globals.sam->element->opt[0] = '\0';
    c2b_globals.sam->element->opt_length = 0;
    if (sam_field_offsets[11] != -1) {
        for (int field_idx = 11; field_idx <= sam_field_idx; field_idx++) {
            ssize_t opt_size = sam_field_offsets[field_idx] - sam_field_offsets[field_idx - 1] - (field_idx == sam_field_idx ? 1 : 0);
            if ((c2b_globals.sam->element->opt_length + opt_size) >= c2b_globals.sam->element->opt_capacity) {
                char *opt_resized = NULL;
                opt_resized = realloc(c2b_globals.sam->element->opt, c2b_globals.sam->element->opt_length + opt_size + 1);
                if (opt_resized) {
                    c2b_globals.sam->element->opt = opt_resized;
                    c2b_globals.sam->element->opt_capacity = c2b_globals.sam->element->opt_length + opt_size + 1;
                }
                else {
                    fprintf(stderr, "Error: Could not resize OPT string in SAM element struct\n");
                    exit(ENOMEM);
                }
            }
            memcpy(c2b_globals.sam->element->opt + c2b_globals.sam->element->opt_length, src + sam_field_offsets[field_idx - 1] + 1, opt_size);
            c2b_globals.sam->element->opt[c2b_globals.sam->element->opt_length + opt_size] = '\0';
            c2b_globals.sam->element->opt_length += opt_size;
        }
    }

    /* 
       Loop through operations and process a line of input based on each operation and its associated value
    */

    for (op_idx = 0; op_idx < c2b_globals.sam->cigar->length; ++op_idx) {
        char current_op = c2b_globals.sam->cigar->ops[op_idx].operation;
        unsigned int bases = c2b_globals.sam->cigar->ops[op_idx].bases;
        switch (current_op) 
            {
            case 'M':
            case 'N':
            case 'D':
            case '=':
            case 'X':
                c2b_globals.sam->element->stop += bases;
            case 'H':
            case 'I':
            case 'P':
            case 'S':
                break;
            default:
                break;
            }
    }

    c2b_line_convert_sam_ptr_to_bed(c2b_globals.sam->element, dest, dest_size, dest_capacity, kFalse);
}

static void
c2b_line_convert_sam_to_bed_unsorted_with_split_operation(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    /* 
       This functor builds a list of tab delimiters, but first reads in the CIGAR string (6th field) and
       parses it for operation key-value pairs to loop through later on
    */

    ssize_t sam_field_offsets[C2B_MAX_FIELD_COUNT_VALUE];
    int sam_field_idx = 0;
    ssize_t current_src_posn = -1;

    /* 
       Find offsets or process header line 
    */

    if (src[0] == c2b_sam_header_prefix) {
        if (!c2b_globals.keep_header_flag) {
            /* skip header line */
            return;
        }
        else {
            /* set up temporary header stream buffers */
            if (!c2b_globals.src_line_str) {
                c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                if (!c2b_globals.src_line_str) {
                    fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                    exit(ENOMEM);
                }
            }
            if (!c2b_globals.dest_line_str) {
                c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
                if (!c2b_globals.dest_line_str) {
                    fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                    exit(ENOMEM);
                }
            }
            /* copy header line to destination stream buffer */
            memcpy(c2b_globals.src_line_str, src, src_size);
            //src_header_line_str[src_size] = '\0';
            sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
            return;
        }
    }

    while (++current_src_posn < src_size) {
        if ((src[current_src_posn] == c2b_tab_delim) || (src[current_src_posn] == c2b_line_delim)) {
            sam_field_offsets[sam_field_idx++] = current_src_posn;
        }
        if (sam_field_idx >= C2B_MAX_FIELD_COUNT_VALUE) {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", sam_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }
    sam_field_offsets[sam_field_idx] = src_size;
    sam_field_offsets[sam_field_idx + 1] = -1;

    /* 
       If no more than one field is read in, then something went wrong
    */

    if (sam_field_idx == 0) {
        fprintf(stderr, "Error: Invalid field count (%d) -- input file may not match input format\n", sam_field_idx);
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

    /* 
       Translate CIGAR string to operations
    */

    ssize_t cigar_size = sam_field_offsets[5] - sam_field_offsets[4];
    if (cigar_size >= c2b_globals.sam->element->cigar_capacity) {
        char *cigar_resized = NULL;
        cigar_resized = realloc(c2b_globals.sam->element->cigar, cigar_size + 1);
        if (cigar_resized) {
            c2b_globals.sam->element->cigar = cigar_resized;
            c2b_globals.sam->element->cigar_capacity = cigar_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize CIGAR string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->cigar, src + sam_field_offsets[4] + 1, cigar_size - 1);
    c2b_globals.sam->element->cigar[cigar_size - 1] = '\0';
    c2b_sam_cigar_str_to_ops(c2b_globals.sam->element->cigar);
#ifdef DEBUG
    c2b_sam_debug_cigar_ops(c2b_globals.sam->cigar);
#endif
    ssize_t cigar_length = 0;
    ssize_t op_idx = 0;
    for (op_idx = 0; op_idx < c2b_globals.sam->cigar->length; ++op_idx) {
        cigar_length += c2b_globals.sam->cigar->ops[op_idx].bases;
    }

    /* 
       Firstly, is the read mapped? If not, and c2b_globals.all_reads_flag is kFalse, we skip over this line
    */

    ssize_t flag_size = sam_field_offsets[1] - sam_field_offsets[0];
    if (flag_size >= c2b_globals.sam->element->flag_str_capacity) {
        char *flag_str_resized = NULL;
        flag_str_resized = realloc(c2b_globals.sam->element->flag_str, flag_size + 1);
        if (flag_str_resized) {
            c2b_globals.sam->element->flag_str = flag_str_resized;
            c2b_globals.sam->element->flag_str_capacity = flag_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize flag string buffer in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->flag_str, src + sam_field_offsets[0] + 1, flag_size);
    c2b_globals.sam->element->flag_str[flag_size] = '\0';
    c2b_globals.sam->element->flag = (int) strtol(c2b_globals.sam->element->flag_str, NULL, 10);
    boolean is_mapped = (boolean) !(4 & c2b_globals.sam->element->flag);
    if ((!is_mapped) && (!c2b_globals.all_reads_flag)) 
        return;   

    /* 
       Secondly, we need to retrieve RNAME, POS, QNAME parameters
    */

    /* RNAME */
    if (is_mapped) {
        ssize_t rname_size = sam_field_offsets[2] - sam_field_offsets[1] - 1;
        if (rname_size >= c2b_globals.sam->element->rname_capacity) {
            char *rname_resized = NULL;
            rname_resized = realloc(c2b_globals.sam->element->rname, rname_size + 1);
            if (rname_resized) {
                c2b_globals.sam->element->rname = rname_resized;
                c2b_globals.sam->element->rname_capacity = rname_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize RNAME string in SAM element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.sam->element->rname, src + sam_field_offsets[1] + 1, rname_size);
        c2b_globals.sam->element->rname[rname_size] = '\0';
    }
    else {
        ssize_t c2b_unmapped_read_chr_name_size = strlen(c2b_unmapped_read_chr_name);
        if (c2b_unmapped_read_chr_name_size >= c2b_globals.sam->element->rname_capacity) {
            char *rname_resized = NULL;
            rname_resized = realloc(c2b_globals.sam->element->rname, c2b_unmapped_read_chr_name_size + 2);
            if (rname_resized) {
                c2b_globals.sam->element->rname = rname_resized;
                c2b_globals.sam->element->rname_capacity = c2b_unmapped_read_chr_name_size + 2;
            }
            else {
                fprintf(stderr, "Error: Could not resize RNAME string in SAM element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.sam->element->rname, c2b_unmapped_read_chr_name, c2b_unmapped_read_chr_name_size);
        c2b_globals.sam->element->rname[c2b_unmapped_read_chr_name_size] = '\t';
        c2b_globals.sam->element->rname[c2b_unmapped_read_chr_name_size + 1] = '\0';
    }

    /* POS */
    ssize_t pos_size = sam_field_offsets[3] - sam_field_offsets[2];
    if (pos_size >= c2b_globals.sam->element->pos_str_capacity) {
        char *pos_str_resized = NULL;
        pos_str_resized = realloc(c2b_globals.sam->element->pos_str, pos_size + 1);
        if (pos_str_resized) {
            c2b_globals.sam->element->pos_str = pos_str_resized;
            c2b_globals.sam->element->pos_str_capacity = pos_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize position string buffer in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->pos_str, src + sam_field_offsets[2] + 1, pos_size);
    c2b_globals.sam->element->pos_str[pos_size] = '\0';
    uint64_t pos_val = strtoull(c2b_globals.sam->element->pos_str, NULL, 10);

    uint64_t start_val = pos_val - 1; /* remember, start = POS - 1 */
    c2b_globals.sam->element->start = start_val;
    c2b_globals.sam->element->stop = start_val; /* this will be adjusted after CIGAR string is parsed */

    /* QNAME */
    ssize_t qname_size = sam_field_offsets[0];
    if (qname_size >= c2b_globals.sam->element->qname_capacity) {
        char *qname_resized = NULL;
        qname_resized = realloc(c2b_globals.sam->element->qname, qname_size + 1);
        if (qname_resized) {
            c2b_globals.sam->element->qname = qname_resized;
            c2b_globals.sam->element->qname_capacity = qname_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize QNAME string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->qname, src, qname_size);
    c2b_globals.sam->element->qname[qname_size] = '\0';

    /* 16 & FLAG */
    int strand_val = 0x10 & c2b_globals.sam->element->flag;
    char strand_str[C2B_MAX_STRAND_LENGTH_VALUE] = {0};
    sprintf(strand_str, "%c", (strand_val == 0x10) ? '-' : '+');
    ssize_t strand_size = strlen(strand_str);
    if (strand_size >= c2b_globals.sam->element->strand_capacity) {
        char *strand_resized = NULL;
        strand_resized = realloc(c2b_globals.sam->element->strand, strand_size + 1);
        if (strand_resized) {
            c2b_globals.sam->element->strand = strand_resized;
            c2b_globals.sam->element->strand_capacity = strand_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize STRAND string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->strand, strand_str, strand_size);
    c2b_globals.sam->element->strand[strand_size] = '\0';
    
    /* MAPQ */
    ssize_t mapq_size = sam_field_offsets[4] - sam_field_offsets[3] - 1;
    if (mapq_size >= c2b_globals.sam->element->mapq_capacity) {
        char *mapq_resized = NULL;
        mapq_resized = realloc(c2b_globals.sam->element->mapq, mapq_size + 1);
        if (mapq_resized) {
            c2b_globals.sam->element->mapq = mapq_resized;
            c2b_globals.sam->element->mapq_capacity = mapq_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize MAPQ string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->mapq, src + sam_field_offsets[3] + 1, mapq_size);
    c2b_globals.sam->element->mapq[mapq_size] = '\0';
    
    /* RNEXT */
    ssize_t rnext_size = sam_field_offsets[6] - sam_field_offsets[5] - 1;
    if (rnext_size >= c2b_globals.sam->element->rnext_capacity) {
        char *rnext_resized = NULL;
        rnext_resized = realloc(c2b_globals.sam->element->rnext, rnext_size + 1);
        if (rnext_resized) {
            c2b_globals.sam->element->rnext = rnext_resized;
            c2b_globals.sam->element->rnext_capacity = rnext_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize RNEXT string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->rnext, src + sam_field_offsets[5] + 1, rnext_size);
    c2b_globals.sam->element->rnext[rnext_size] = '\0';

    /* PNEXT */
    ssize_t pnext_size = sam_field_offsets[7] - sam_field_offsets[6] - 1;
    if (pnext_size >= c2b_globals.sam->element->pnext_capacity) {
        char *pnext_resized = NULL;
        pnext_resized = realloc(c2b_globals.sam->element->pnext, pnext_size + 1);
        if (pnext_resized) {
            c2b_globals.sam->element->pnext = pnext_resized;
            c2b_globals.sam->element->pnext_capacity = pnext_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize PNEXT string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->pnext, src + sam_field_offsets[6] + 1, pnext_size);
    c2b_globals.sam->element->pnext[pnext_size] = '\0';

    /* TLEN */
    ssize_t tlen_size = sam_field_offsets[8] - sam_field_offsets[7] - 1;
    if (tlen_size >= c2b_globals.sam->element->tlen_capacity) {
        char *tlen_resized = NULL;
        tlen_resized = realloc(c2b_globals.sam->element->tlen, tlen_size + 1);
        if (tlen_resized) {
            c2b_globals.sam->element->tlen = tlen_resized;
            c2b_globals.sam->element->tlen_capacity = tlen_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize TLEN string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->tlen, src + sam_field_offsets[7] + 1, tlen_size);
    c2b_globals.sam->element->tlen[tlen_size] = '\0';

    /* SEQ */
    ssize_t seq_size = sam_field_offsets[9] - sam_field_offsets[8] - 1;
    if (seq_size >= c2b_globals.sam->element->seq_capacity) {
        char *seq_resized = NULL;
        seq_resized = realloc(c2b_globals.sam->element->seq, seq_size + 1);
        if (seq_resized) {
            c2b_globals.sam->element->seq = seq_resized;
            c2b_globals.sam->element->seq_capacity = seq_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize SEQ string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->seq, src + sam_field_offsets[8] + 1, seq_size);
    c2b_globals.sam->element->seq[seq_size] = '\0';

    /* QUAL */
    ssize_t qual_size = sam_field_offsets[10] - sam_field_offsets[9] - 1;
    if (qual_size >= c2b_globals.sam->element->qual_capacity) {
        char *qual_resized = NULL;
        qual_resized = realloc(c2b_globals.sam->element->qual, qual_size + 1);
        if (qual_resized) {
            c2b_globals.sam->element->qual = qual_resized;
            c2b_globals.sam->element->qual_capacity = qual_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize QUAL string in SAM element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.sam->element->qual, src + sam_field_offsets[9] + 1, qual_size);
    c2b_globals.sam->element->qual[qual_size] = '\0';

    c2b_globals.sam->element->opt[0] = '\0';
    c2b_globals.sam->element->opt_length = 0;
    if (sam_field_offsets[11] != -1) {
        for (int field_idx = 11; field_idx <= sam_field_idx; field_idx++) {
            ssize_t opt_size = sam_field_offsets[field_idx] - sam_field_offsets[field_idx - 1] - (field_idx == sam_field_idx ? 1 : 0);
            if ((c2b_globals.sam->element->opt_length + opt_size) >= c2b_globals.sam->element->opt_capacity) {
                char *opt_resized = NULL;
                opt_resized = realloc(c2b_globals.sam->element->opt, c2b_globals.sam->element->opt_length + opt_size + 1);
                if (opt_resized) {
                    c2b_globals.sam->element->opt = opt_resized;
                    c2b_globals.sam->element->opt_capacity = c2b_globals.sam->element->opt_length + opt_size + 1;
                }
                else {
                    fprintf(stderr, "Error: Could not resize OPT string in SAM element struct\n");
                    exit(ENOMEM);
                }
            }
            memcpy(c2b_globals.sam->element->opt + c2b_globals.sam->element->opt_length, src + sam_field_offsets[field_idx - 1] + 1, opt_size);
            c2b_globals.sam->element->opt[c2b_globals.sam->element->opt_length + opt_size] = '\0';
            c2b_globals.sam->element->opt_length += opt_size;
        }
    }

    /* 
       Loop through operations and process a line of input based on each operation and its associated value
    */

    ssize_t block_idx;
    char previous_op = default_cigar_op_operation;

    for (op_idx = 0, block_idx = 1; op_idx < c2b_globals.sam->cigar->length; ++op_idx) {
        char current_op = c2b_globals.sam->cigar->ops[op_idx].operation;
        char next_op = '\0';
        if (op_idx != (c2b_globals.sam->cigar->length - 1)) {
            next_op = c2b_globals.sam->cigar->ops[op_idx + 1].operation;
        }
        unsigned int bases = c2b_globals.sam->cigar->ops[op_idx].bases;
        if (c2b_globals.split_with_deletions_flag) {
            switch (current_op) 
                {
                case 'M':
                    c2b_globals.sam->element->stop += bases;
                    if ((previous_op == default_cigar_op_operation) || (previous_op == 'D') || (previous_op == 'N')) {
                        ssize_t desired_modified_qname_capacity = c2b_globals.sam->element->qname_capacity + C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_EXTENSION;
                        if (c2b_globals.sam->element->modified_qname_capacity <= desired_modified_qname_capacity) {
                            // resize modified qname capacity to fit
                            char *modified_qname_resized = NULL;
                            modified_qname_resized = realloc(c2b_globals.sam->element->modified_qname, desired_modified_qname_capacity + 1);
                            if (modified_qname_resized) {
                                c2b_globals.sam->element->modified_qname = modified_qname_resized;
                                c2b_globals.sam->element->modified_qname_capacity = desired_modified_qname_capacity + 1;
                            }
                            else {
                                fprintf(stderr, "Error: Could not resize modified QNAME string in SAM element struct\n");
                                exit(ENOMEM);
                            }
                        }
                        // block_idx string can be up to (C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_EXTENSION-1) characters long
                        sprintf(c2b_globals.sam->element->modified_qname, "%s/%zu", c2b_globals.sam->element->qname, block_idx++);
                        c2b_line_convert_sam_ptr_to_bed(c2b_globals.sam->element, dest, dest_size, dest_capacity, kTrue);
                        c2b_globals.sam->element->start = c2b_globals.sam->element->stop;
                    }
                    break;
                case 'N':
                case 'D':
                    c2b_globals.sam->element->stop += bases;
                    c2b_globals.sam->element->start = c2b_globals.sam->element->stop;
                case 'H':
                case 'I':
                case 'P':
                case 'S':
                    break;
                default:
                    break;
                }
        }
        else {
            switch (current_op) 
                {
                case 'M':
                    c2b_globals.sam->element->stop += bases;
                    if ((previous_op == default_cigar_op_operation) || (previous_op == 'D') || (previous_op == 'N')) {
                        ssize_t desired_modified_qname_capacity = c2b_globals.sam->element->qname_capacity + C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_EXTENSION;
                        if (c2b_globals.sam->element->modified_qname_capacity <= desired_modified_qname_capacity) {
                            // resize modified qname capacity to fit
                            char *modified_qname_resized = NULL;
                            modified_qname_resized = realloc(c2b_globals.sam->element->modified_qname, desired_modified_qname_capacity + 1);
                            if (modified_qname_resized) {
                                c2b_globals.sam->element->modified_qname = modified_qname_resized;
                                c2b_globals.sam->element->modified_qname_capacity = desired_modified_qname_capacity + 1;
                            }
                            else {
                                fprintf(stderr, "Error: Could not resize modified QNAME string in SAM element struct\n");
                                exit(ENOMEM);
                            }
                        }
                        if ((next_op == 'N') || (next_op == '\0')) {
                            // block_idx string can be up to (C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_EXTENSION-1) characters long
                            sprintf(c2b_globals.sam->element->modified_qname, "%s/%zu", c2b_globals.sam->element->qname, block_idx++);
                            c2b_line_convert_sam_ptr_to_bed(c2b_globals.sam->element, dest, dest_size, dest_capacity, kTrue);
                            c2b_globals.sam->element->start = c2b_globals.sam->element->stop;
                        }
                    }
                    break;
                case 'N':
                    c2b_globals.sam->element->stop += bases;
                    c2b_globals.sam->element->start = c2b_globals.sam->element->stop;
                    break;
                case 'D':
                    c2b_globals.sam->element->stop += bases;
                    break;
                case 'H':
                case 'I':
                case 'P':
                case 'S':
                    break;
                default:
                    break;
                }
        }
        previous_op = current_op;
    }

    /* 
       If the CIGAR string does not contain a split or deletion operation ('N', 'D') or the
       operations are unavailable ('*') then to quote Captain John O'Hagan, we don't enhance: we 
       just print the damn thing
    */

    if (block_idx == 1) {
        c2b_line_convert_sam_ptr_to_bed(c2b_globals.sam->element, dest, dest_size, dest_capacity, kFalse);
    }
}

static inline void
c2b_sam_cigar_str_to_ops(char* s)
{
    size_t s_idx;
    size_t s_len = strlen(s);
    size_t bases_idx = 0;
    boolean bases_flag = kTrue;
    boolean operation_flag = kFalse;
    char curr_bases_field[C2B_MAX_OPERATION_FIELD_LENGTH_VALUE + 1];
    char curr_char = default_cigar_op_operation;
    unsigned int curr_bases = 0;
    ssize_t op_idx = 0;

    for (s_idx = 0; s_idx < s_len; ++s_idx) {
        curr_char = s[s_idx];
        if (isdigit(curr_char)) {
            if (operation_flag) {
                c2b_globals.sam->cigar->ops[op_idx].bases = curr_bases;
                op_idx++;
                operation_flag = kFalse;
                bases_flag = kTrue;
                /* if op_idx >= size property of CIGAR entity, we need to resize this entity */
                if (op_idx >= c2b_globals.sam->cigar->size) {
                    c2b_cigar_t *resized_cigar = NULL;
                    c2b_sam_resize_cigar_ops(&resized_cigar, c2b_globals.sam->cigar);
                    c2b_sam_delete_cigar_ops(c2b_globals.sam->cigar);
                    c2b_globals.sam->cigar = resized_cigar;
                }
            }
            curr_bases_field[bases_idx++] = curr_char;
            curr_bases_field[bases_idx] = '\0';
        }
        else {
            if (bases_flag) {
                curr_bases = atoi(curr_bases_field);
                bases_flag = kFalse;
                operation_flag = kTrue;
                bases_idx = 0;
                memset(curr_bases_field, 0, strlen(curr_bases_field));
            }
            c2b_globals.sam->cigar->ops[op_idx].operation = curr_char;
            if (curr_char == '*') {
                break;
            }
        }
    }
    c2b_globals.sam->cigar->ops[op_idx].bases = curr_bases;
    c2b_globals.sam->cigar->length = op_idx + 1;
}

static void
c2b_sam_init_element(c2b_sam_t** e)
{
    *e = malloc(sizeof(c2b_sam_t));
    if (!*e) {
        fprintf(stderr, "Error: Could not allocate space for SAM element pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    
    (*e)->qname = NULL, (*e)->qname = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qname)));
    if (!(*e)->qname) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element qname malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qname_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->modified_qname = NULL, (*e)->modified_qname = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->modified_qname)));
    if (!(*e)->modified_qname) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element modified qname malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->modified_qname_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->strand = NULL, (*e)->strand = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->strand)));
    if (!(*e)->strand) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element strand malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->strand_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->rname = NULL, (*e)->rname = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->rname)));
    if (!(*e)->rname) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element rname malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->rname_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->mapq = NULL, (*e)->mapq = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->mapq)));
    if (!(*e)->mapq) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element mapq malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->mapq_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->cigar = NULL, (*e)->cigar = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->cigar)));
    if (!(*e)->cigar) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element cigar malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->cigar_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->rnext = NULL, (*e)->rnext = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->rnext)));
    if (!(*e)->rnext) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element rnext malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->rnext_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->pnext = NULL, (*e)->pnext = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->pnext)));
    if (!(*e)->pnext) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element pnext malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->pnext_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->tlen = NULL, (*e)->tlen = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->tlen)));
    if (!(*e)->tlen) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element tlen malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->tlen_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->seq = NULL, (*e)->seq = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->seq)));
    if (!(*e)->seq) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element seq malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->seq_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qual = NULL, (*e)->qual = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qual)));
    if (!(*e)->qual) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element qual malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qual_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->opt = NULL, (*e)->opt = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->opt)));
    if (!(*e)->opt) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element opt malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->opt_length = 0;
    (*e)->opt_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->flag = 0;
    (*e)->flag_str = NULL, (*e)->flag_str = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->flag_str)));
    if (!(*e)->flag_str) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element flag string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->flag_str_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->start = 0;
    (*e)->stop = 0;
    (*e)->pos_str = NULL, (*e)->pos_str = malloc(C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->pos_str)));
    if (!(*e)->pos_str) { 
        fprintf(stderr, "Error: Could not allocate space for SAM element position string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->pos_str_capacity = C2B_SAM_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;
}

static void
c2b_sam_delete_element(c2b_sam_t* e) 
{
    if (e->qname)           { free(e->qname),           e->qname = NULL;           }
    if (e->modified_qname)  { free(e->modified_qname),  e->modified_qname = NULL;  }
    if (e->strand)          { free(e->strand),          e->strand = NULL;          }
    if (e->rname)           { free(e->rname),           e->rname = NULL;           }
    if (e->mapq)            { free(e->mapq),            e->mapq = NULL;            }
    if (e->cigar)           { free(e->cigar),           e->cigar = NULL;           }
    if (e->rnext)           { free(e->rnext),           e->rnext = NULL;           }
    if (e->pnext)           { free(e->pnext),           e->pnext = NULL;           }
    if (e->tlen)            { free(e->tlen),            e->tlen = NULL;            }
    if (e->seq)             { free(e->seq),             e->seq = NULL;             }
    if (e->qual)            { free(e->qual),            e->qual = NULL;            }
    if (e->opt)             { free(e->opt),             e->opt = NULL;             }
    if (e->flag_str)        { free(e->flag_str),        e->flag_str = NULL;        }
    if (e->pos_str)         { free(e->pos_str),         e->pos_str = NULL;         }
    if (e)                  { free(e),                  e = NULL;                  }
}

static void
c2b_sam_init_cigar_ops(c2b_cigar_t** c, const ssize_t size)
{
    *c = malloc(sizeof(c2b_cigar_t));
    if (!*c) {
        fprintf(stderr, "Error: Could not allocate space for SAM CIGAR struct pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*c)->size = size;
    (*c)->ops = malloc((*c)->size * sizeof(c2b_cigar_op_t));
    if (!(*c)->ops) {
        fprintf(stderr, "Error: Could not allocate space for SAM CIGAR struct malloc operation pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*c)->length = 0;
    for (ssize_t idx = 0; idx < size; idx++) {
        (*c)->ops[idx].bases = default_cigar_op_bases;
        (*c)->ops[idx].operation = default_cigar_op_operation;
    }
}

static void
c2b_sam_resize_cigar_ops(c2b_cigar_t** new_c, c2b_cigar_t* old_c)
{
    *new_c = malloc(sizeof(c2b_cigar_t));
    if (!*new_c) {
        fprintf(stderr, "Error: Could not allocate space for larger SAM CIGAR struct pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    /* we increment the size of the input CIGAR entity by C2B_SAM_CIGAR_OPS_VALUE_INCREMENT */
    (*new_c)->size = old_c->size + C2B_SAM_CIGAR_OPS_VALUE_INCREMENT;
    (*new_c)->ops = malloc((*new_c)->size * sizeof(c2b_cigar_op_t));
    if (!(*new_c)->ops) {
        fprintf(stderr, "Error: Could not allocate space for larger SAM CIGAR struct malloc operation pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*new_c)->length = old_c->length;
    /* copy operations from input CIGAR entity to new entity */
    for (ssize_t idx = 0; idx < old_c->size; idx++) {
        (*new_c)->ops[idx].bases = old_c->ops[idx].bases;
        (*new_c)->ops[idx].operation = old_c->ops[idx].operation;
    }
    /* set default base and operation values for new entity */
    for (ssize_t idx = old_c->size; idx < (*new_c)->size; idx++) {
        (*new_c)->ops[idx].bases = default_cigar_op_bases;
        (*new_c)->ops[idx].operation = default_cigar_op_operation;
    }
}

/* 
   specifying special attribute for c2b_sam_debug_cigar_ops() to avoid: "warning: unused 
   function 'c2b_sam_debug_cigar_ops' [-Wunused-function]" message during non-debug compilation

   cf. http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Function-Attributes.html#Function%20Attributes
*/
#if defined(__GNUC__)
static void c2b_sam_debug_cigar_ops() __attribute__ ((unused));
#endif

static void
c2b_sam_debug_cigar_ops(c2b_cigar_t* c)
{
    ssize_t idx = 0;
    ssize_t length = c->length;
    for (idx = 0; idx < length; ++idx) {
        fprintf(stderr, "\t-> c2b_sam_debug_cigar_ops - %zu [%03u, %c]\n", idx, c->ops[idx].bases, c->ops[idx].operation);
    }
}

static void
c2b_sam_delete_cigar_ops(c2b_cigar_t* c)
{
    if (c) {
        if (c->ops) {
            free(c->ops), c->ops = NULL; 
        }
        c->length = 0;
        c->size = 0;
        free(c), c = NULL;
    }
}

static inline void
c2b_line_convert_sam_ptr_to_bed(c2b_sam_t* s, char** dest_line_ptr, ssize_t* dest_size, ssize_t* dest_capacity, boolean print_modified_qname)
{
    
    /*
       For SAM-formatted data, we use the mapping provided by BEDOPS convention described at: 

       http://bedops.readthedocs.io/en/latest/content/reference/file-management/conversion/sam2bed.html

       SAM field                 BED column index       BED field
       -------------------------------------------------------------------------
       RNAME                     1                      chromosome
       POS - 1                   2                      start
       POS + length(SEQ) - 1     3                      stop
       QNAME                     4                      id
       MAPQ                      5                      score
       16 & FLAG                 6                      strand

       If NOT (4 & FLAG) is true, then the read is mapped.

       If the --reduced option is not specified, the remaining SAM columns are mapped as-is, 
       in same order, to adjacent BED columns:

       SAM field                 BED column index       BED field
       -------------------------------------------------------------------------
       FLAG                      7                      -
       CIGAR                     8                      -
       RNEXT                     9                      -
       PNEXT                     10                     -
       TLEN                      11                     -
       SEQ                       12                     -
       QUAL                      13                     -

       If present:

       SAM field                 BED column index       BED field
       -------------------------------------------------------------------------
       Alignment fields          14+                    -

       If the --reduced option is specified, only the first six columns are printed.
    */

    if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
        char *dest_line_resized = NULL;
        dest_line_resized = realloc(*dest_line_ptr, *dest_capacity * 2);
        if (dest_line_resized) {
            *dest_capacity *= 2;
            *dest_line_ptr = dest_line_resized;
        }
        else {
            fprintf(stderr, "Error: Could not resize dest_line_ptr string in SAM pointer conversion fn\n");
            exit(ENOMEM);
        }
    }

    if ((!c2b_globals.reduced_flag) && (s->opt_length > 0)) {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%d\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              s->rname,
                              s->start,
                              s->stop,
                              (print_modified_qname ? s->modified_qname : s->qname),
                              s->mapq,
                              s->strand,
                              s->flag,
                              s->cigar,
                              s->rnext,
                              s->pnext,
                              s->tlen,
                              s->seq,
                              s->qual,
                              s->opt);
    } 
    else if ((!c2b_globals.reduced_flag) && (s->opt_length == 0)) {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%d\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              s->rname,
                              s->start,
                              s->stop,
                              (print_modified_qname ? s->modified_qname : s->qname),
                              s->mapq,
                              s->strand,
                              s->flag,
                              s->cigar,
                              s->rnext,
                              s->pnext,
                              s->tlen,
                              s->seq,
                              s->qual);
    }
    else if (c2b_globals.reduced_flag) {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              s->rname,
                              s->start,
                              s->stop,
                              (print_modified_qname ? s->modified_qname : s->qname),
                              s->mapq,
                              s->strand);
    }
}

static void
c2b_line_convert_vcf_to_bed_unsorted(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    ssize_t vcf_field_offsets[C2B_MAX_VCF_FIELD_COUNT_VALUE];
    int vcf_field_idx = 0;
    ssize_t current_src_posn = -1;

    while (++current_src_posn < src_size) {
        if ((src[current_src_posn] == c2b_tab_delim) || (src[current_src_posn] == c2b_line_delim)) {
            vcf_field_offsets[vcf_field_idx++] = current_src_posn;
        }
        if (vcf_field_idx >= C2B_MAX_VCF_FIELD_COUNT_VALUE) {
            fprintf(stderr, "Error: Invalid field count (%d) -- input file may have too many fields\n", vcf_field_idx);
            c2b_print_usage(stderr);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
    }
    vcf_field_offsets[vcf_field_idx] = src_size;
    vcf_field_offsets[vcf_field_idx + 1] = -1;

    /* 
       If number of fields in not in bounds, we may need to exit early
    */

    /* 0 - CHROM */
    ssize_t chrom_size = vcf_field_offsets[0];
    if (chrom_size >= c2b_globals.vcf->element->chrom_capacity) {
        char *chrom_resized = NULL;
        chrom_resized = realloc(c2b_globals.vcf->element->chrom, chrom_size + 1);
        if (chrom_resized) {
            c2b_globals.vcf->element->chrom = chrom_resized;
            c2b_globals.vcf->element->chrom_capacity = chrom_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize CHROM string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->chrom, src, chrom_size);
    c2b_globals.vcf->element->chrom[chrom_size] = '\0';

    if ((c2b_globals.vcf->element->chrom[0] == c2b_vcf_header_prefix) && (c2b_globals.keep_header_flag)) {
        if (!c2b_globals.src_line_str) {
            c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
            if (!c2b_globals.src_line_str) {
                fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
                exit(ENOMEM);
            }
        }
        if (!c2b_globals.dest_line_str) {
            c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
            if (!c2b_globals.dest_line_str) {
                fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.src_line_str, src, src_size);
        c2b_globals.src_line_str[src_size] = '\0';
        sprintf(c2b_globals.dest_line_str, "%s\t%u\t%u\t%s\n", c2b_header_chr_name, c2b_globals.header_line_idx, (c2b_globals.header_line_idx + 1), c2b_globals.src_line_str);
        memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
        *dest_size += strlen(c2b_globals.dest_line_str);
        c2b_globals.header_line_idx++;
        return;
    }
    else if (c2b_globals.vcf->element->chrom[0] == c2b_vcf_header_prefix) {
        return;
    }

    /* 1 - POS */
    ssize_t pos_size = vcf_field_offsets[1] - vcf_field_offsets[0] - 1;
    if (pos_size >= c2b_globals.vcf->element->pos_str_capacity) {
        char *pos_str_resized = NULL;
        pos_str_resized = realloc(c2b_globals.vcf->element->pos_str, pos_size + 1);
        if (pos_str_resized) {
            c2b_globals.vcf->element->pos_str = pos_str_resized;
            c2b_globals.vcf->element->pos_str_capacity = pos_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize position string buffer in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->pos_str, src + vcf_field_offsets[0] + 1, pos_size);
    c2b_globals.vcf->element->pos_str[pos_size] = '\0';
    c2b_globals.vcf->element->pos = strtoull(c2b_globals.vcf->element->pos_str, NULL, 10);

    c2b_globals.vcf->element->start = c2b_globals.vcf->element->pos - 1;
    c2b_globals.vcf->element->end = c2b_globals.vcf->element->pos; /* note that this value may change below, depending on options */

    /* 2 - ID */
    ssize_t id_size = vcf_field_offsets[2] - vcf_field_offsets[1] - 1;
    if (id_size >= c2b_globals.vcf->element->id_capacity) {
        char *id_resized = NULL;
        id_resized = realloc(c2b_globals.vcf->element->id, id_size + 1);
        if (id_resized) {
            c2b_globals.vcf->element->id = id_resized;
            c2b_globals.vcf->element->id_capacity = id_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize ID string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->id, src + vcf_field_offsets[1] + 1, id_size);
    c2b_globals.vcf->element->id[id_size] = '\0';

    /* 3 - REF */
    ssize_t ref_size = vcf_field_offsets[3] - vcf_field_offsets[2] - 1;
    if (ref_size >= c2b_globals.vcf->element->ref_capacity) {
        char *ref_resized = NULL;
        ref_resized = realloc(c2b_globals.vcf->element->ref, ref_size + 1);
        if (ref_resized) {
            c2b_globals.vcf->element->ref = ref_resized;
            c2b_globals.vcf->element->ref_capacity = ref_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize REF string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->ref, src + vcf_field_offsets[2] + 1, ref_size);
    c2b_globals.vcf->element->ref[ref_size] = '\0';

    /* 4 - ALT */
    ssize_t alt_size = vcf_field_offsets[4] - vcf_field_offsets[3] - 1;
    if (alt_size >= c2b_globals.vcf->element->alt_capacity) {
        char *alt_resized = NULL;
        alt_resized = realloc(c2b_globals.vcf->element->alt, alt_size + 1);
        if (alt_resized) {
            c2b_globals.vcf->element->alt = alt_resized;
            c2b_globals.vcf->element->alt_capacity = alt_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize ALT string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->alt, src + vcf_field_offsets[3] + 1, alt_size);
    c2b_globals.vcf->element->alt[alt_size] = '\0';

    /* 5 - QUAL */
    ssize_t qual_size = vcf_field_offsets[5] - vcf_field_offsets[4] - 1;
    if (qual_size >= c2b_globals.vcf->element->qual_capacity) {
        char *qual_resized = NULL;
        qual_resized = realloc(c2b_globals.vcf->element->qual, qual_size + 1);
        if (qual_resized) {
            c2b_globals.vcf->element->qual = qual_resized;
            c2b_globals.vcf->element->qual_capacity = qual_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize QUAL string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->qual, src + vcf_field_offsets[4] + 1, qual_size);
    c2b_globals.vcf->element->qual[qual_size] = '\0';

    /* 6 - FILTER */
    ssize_t filter_size = vcf_field_offsets[6] - vcf_field_offsets[5] - 1;
    if (filter_size >= c2b_globals.vcf->element->filter_capacity) {
        char *filter_resized = NULL;
        filter_resized = realloc(c2b_globals.vcf->element->filter, filter_size + 1);
        if (filter_resized) {
            c2b_globals.vcf->element->filter = filter_resized;
            c2b_globals.vcf->element->filter_capacity = filter_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize FILTER string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->filter, src + vcf_field_offsets[5] + 1, filter_size);
    c2b_globals.vcf->element->filter[filter_size] = '\0';

    /* 7 - INFO */
    ssize_t info_size = vcf_field_offsets[7] - vcf_field_offsets[6] - 1;
    if (info_size >= c2b_globals.vcf->element->info_capacity) {
        char *info_resized = NULL;
        info_resized = realloc(c2b_globals.vcf->element->info, info_size + 1);
        if (info_resized) {
            c2b_globals.vcf->element->info = info_resized;
            c2b_globals.vcf->element->info_capacity = info_size + 1;
        }
        else {
            fprintf(stderr, "Error: Could not resize INFO string in VCF element struct\n");
            exit(ENOMEM);
        }
    }
    memcpy(c2b_globals.vcf->element->info, src + vcf_field_offsets[6] + 1, info_size);
    c2b_globals.vcf->element->info[info_size] = '\0';

    if (vcf_field_idx >= 8) {
        /* 8 - FORMAT */
        ssize_t format_size = vcf_field_offsets[8] - vcf_field_offsets[7] - 1;
        if (format_size >= c2b_globals.vcf->element->format_capacity) {
            char *format_resized = NULL;
            format_resized = realloc(c2b_globals.vcf->element->format, format_size + 1);
            if (format_resized) {
                c2b_globals.vcf->element->format = format_resized;
                c2b_globals.vcf->element->format_capacity = format_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize FORMAT string in VCF element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.vcf->element->format, src + vcf_field_offsets[7] + 1, format_size);
        c2b_globals.vcf->element->format[format_size] = '\0';

        /* 9 - Samples */
        ssize_t samples_size = vcf_field_offsets[vcf_field_idx] - vcf_field_offsets[8] - 1;
        if (samples_size >= c2b_globals.vcf->element->samples_capacity) {
            char *samples_resized = NULL;
            samples_resized = realloc(c2b_globals.vcf->element->samples, samples_size + 1);
            if (samples_resized) {
                c2b_globals.vcf->element->samples = samples_resized;
                c2b_globals.vcf->element->samples_capacity = samples_size + 1;
            }
            else {
                fprintf(stderr, "Error: Could not resize SAMPLES string in VCF element struct\n");
                exit(ENOMEM);
            }
        }
        memcpy(c2b_globals.vcf->element->samples, src + vcf_field_offsets[8] + 1, samples_size);
        c2b_globals.vcf->element->samples[samples_size] = '\0';
    }

    if ((!c2b_globals.vcf->do_not_split) && (memchr(c2b_globals.vcf->element->alt, c2b_vcf_alt_allele_delim, strlen(c2b_globals.vcf->element->alt)))) {
        
        /* loop through each allele */

        char *alt_alleles_copy = NULL;
        alt_alleles_copy = malloc(strlen(c2b_globals.vcf->element->alt) + 1);
        if (!alt_alleles_copy) {
            fprintf(stderr, "Error: Could not allocate space for VCF ALT alleles copy\n");
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(alt_alleles_copy, c2b_globals.vcf->element->alt, strlen(c2b_globals.vcf->element->alt) + 1);
        const char *allele_tok;
        while ((allele_tok = c2b_strsep(&alt_alleles_copy, ",")) != NULL) {
            memcpy(c2b_globals.vcf->element->alt, allele_tok, strlen(allele_tok));
            c2b_globals.vcf->element->alt[strlen(allele_tok)] = '\0';
            if ((c2b_globals.vcf->filter_count == 1) && (!c2b_globals.vcf->only_insertions)) {
                c2b_globals.vcf->element->end = c2b_globals.vcf->element->start + ref_size - strlen(c2b_globals.vcf->element->alt) + 1;
            }
            if ( (c2b_globals.vcf->filter_count == 0) ||
                 ((c2b_globals.vcf->only_snvs) && (c2b_vcf_record_is_snv(c2b_globals.vcf->element->ref, c2b_globals.vcf->element->alt))) ||
                 ((c2b_globals.vcf->only_insertions) && (c2b_vcf_record_is_insertion(c2b_globals.vcf->element->ref, c2b_globals.vcf->element->alt))) ||
                 ((c2b_globals.vcf->only_deletions) && (c2b_vcf_record_is_deletion(c2b_globals.vcf->element->ref, c2b_globals.vcf->element->alt))) ) 
                {
                    c2b_line_convert_vcf_ptr_to_bed(c2b_globals.vcf->element, dest, dest_size, dest_capacity);
                }
        }
        free(alt_alleles_copy), alt_alleles_copy = NULL;
    }
    else {
        
        /* just print the one allele */

        if ((c2b_globals.vcf->filter_count == 1) && (!c2b_globals.vcf->only_insertions)) {
            c2b_globals.vcf->element->end = c2b_globals.vcf->element->start + ref_size - strlen(c2b_globals.vcf->element->alt) + 1;
        }
        if ( (c2b_globals.vcf->filter_count == 0) ||
             ((c2b_globals.vcf->only_snvs) && (c2b_vcf_record_is_snv(c2b_globals.vcf->element->ref, c2b_globals.vcf->element->alt))) ||
             ((c2b_globals.vcf->only_insertions) && (c2b_vcf_record_is_insertion(c2b_globals.vcf->element->ref, c2b_globals.vcf->element->alt))) ||
             ((c2b_globals.vcf->only_deletions) && (c2b_vcf_record_is_deletion(c2b_globals.vcf->element->ref, c2b_globals.vcf->element->alt))) ) 
            {
                c2b_line_convert_vcf_ptr_to_bed(c2b_globals.vcf->element, dest, dest_size, dest_capacity);
            }
    }
}

static inline boolean
c2b_vcf_allele_is_id(char* s)
{
    return ((s[0] == c2b_vcf_id_prefix) && (s[strlen(s)-1] == c2b_vcf_id_suffix)) ? kTrue : kFalse;
}

static inline boolean
c2b_vcf_record_is_snv(char* ref, char* alt) 
{
    return ((!c2b_vcf_allele_is_id(alt)) && (((int) strlen(ref) - (int) strlen(alt)) == 0)) ? kTrue : kFalse;
}

static inline boolean
c2b_vcf_record_is_insertion(char* ref, char* alt) 
{
    return ((!c2b_vcf_allele_is_id(alt)) && (((int) strlen(ref) - (int) strlen(alt)) < 0)) ? kTrue : kFalse;
}

static inline boolean
c2b_vcf_record_is_deletion(char* ref, char* alt) 
{
    return ((!c2b_vcf_allele_is_id(alt)) && (((int) strlen(ref) - (int) strlen(alt)) > 0)) ? kTrue : kFalse;
}

static void
c2b_vcf_init_element(c2b_vcf_t** e)
{
    *e = malloc(sizeof(c2b_vcf_t));
    if (!*e) {
        fprintf(stderr, "Error: Could not allocate space for VCF element pointer\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    
    (*e)->chrom = NULL, (*e)->chrom = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->chrom)));
    if (!(*e)->chrom) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element chrom malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->chrom_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->pos = 0;
    (*e)->pos_str = NULL, (*e)->pos_str = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->pos_str)));
    if (!(*e)->pos_str) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element position string buffer malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->pos_str_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->start = 0;
    (*e)->end = 0;

    (*e)->id = NULL, (*e)->id = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->id)));
    if (!(*e)->id) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element id malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->id_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->ref = NULL, (*e)->ref = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->ref)));
    if (!(*e)->ref) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element ref malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->ref_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->alt = NULL, (*e)->alt = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->alt)));
    if (!(*e)->alt) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element alt malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->alt_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->qual = NULL, (*e)->qual = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->qual)));
    if (!(*e)->qual) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element qual malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->qual_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->filter = NULL, (*e)->filter = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->filter)));
    if (!(*e)->filter) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element filter malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->filter_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->info = NULL, (*e)->info = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->info)));
    if (!(*e)->info) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element info malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->info_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->format = NULL, (*e)->format = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->format)));
    if (!(*e)->format) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element format malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->format_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;

    (*e)->samples = NULL, (*e)->samples = malloc(C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL * sizeof(*((*e)->samples)));
    if (!(*e)->samples) { 
        fprintf(stderr, "Error: Could not allocate space for VCF element samples malloc operation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    (*e)->samples_capacity = C2B_VCF_ELEMENT_FIELD_LENGTH_VALUE_INITIAL;
}

static void
c2b_vcf_delete_element(c2b_vcf_t* e)
{
    if (e->chrom)           { free(e->chrom),           e->chrom = NULL;           }
    if (e->pos_str)         { free(e->pos_str),         e->pos_str = NULL;         }
    if (e->id)              { free(e->id),              e->id = NULL;              }
    if (e->ref)             { free(e->ref),             e->ref = NULL;             }
    if (e->alt)             { free(e->alt),             e->alt = NULL;             }
    if (e->qual)            { free(e->qual),            e->qual = NULL;            }
    if (e->filter)          { free(e->filter),          e->filter = NULL;          }
    if (e->info)            { free(e->info),            e->info = NULL;            }
    if (e->format)          { free(e->format),          e->format = NULL;          }
    if (e->samples)         { free(e->samples),         e->samples = NULL;         }
    if (e)                  { free(e),                  e = NULL;                  }
}

static inline void
c2b_line_convert_vcf_ptr_to_bed(c2b_vcf_t* v, char** dest_line_ptr, ssize_t* dest_size, ssize_t* dest_capacity) 
{
    /* 
       For VCF v4.2-formatted data, we use the mapping provided by BEDOPS convention described at:

       http://bedops.readthedocs.io/en/latest/content/reference/file-management/conversion/vcf2bed.html

       VCF field                 BED column index       BED field
       -------------------------------------------------------------------------
       CHROM                     1                      chromosome
       POS - 1                   2                      start
       POS (*)                   3                      stop
       ID                        4                      id
       QUAL                      5                      score

      * : When using --deletions, the stop value of the BED output is determined by the length difference 
          between ALT and REF alleles. Use of --insertions or --snvs yields a one-base BED element.

       The remaining VCF columns are mapped as-is, in same order, to adjacent BED columns:

       VCF field                 BED column index       BED field
       -------------------------------------------------------------------------
       REF                       6                      -
       ALT                       7                      -
       FILTER                    8                      -
       INFO                      9                      -

       If present in the VCF v4 input, the following columns are also mapped:

       VCF field                 BED column index       BED field
       -------------------------------------------------------------------------
       FORMAT                    10                     -
       Sample 1                  11                     -
       Sample 2                  12                     -
       ...
    */

    if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
        char *dest_line_resized = NULL;
        dest_line_resized = realloc(*dest_line_ptr, *dest_capacity * 2);
        if (dest_line_resized) {
            *dest_capacity *= 2;
            *dest_line_ptr = dest_line_resized;
        }
        else {
            fprintf(stderr, "Error: Could not resize dest_line_ptr string in VCF pointer conversion fn\n");
            exit(ENOMEM);
        }
    }

    if (strlen(v->format) > 0) {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              v->chrom,
                              v->start,
                              v->end,
                              v->id,
                              v->qual,
                              v->ref,
                              v->alt,
                              v->filter,
                              v->info,
                              v->format,
                              v->samples);
    }
    else {
        *dest_size += sprintf(*dest_line_ptr + *dest_size,
                              "%s\t"            \
                              "%" PRIu64 "\t"   \
                              "%" PRIu64 "\t"   \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\t"            \
                              "%s\n",
                              v->chrom,
                              v->start,
                              v->end,
                              v->id,
                              v->qual,
                              v->ref,
                              v->alt,
                              v->filter,
                              v->info);
    }
}

static void
c2b_line_convert_wig_to_bed_unsorted(char** dest, ssize_t* dest_size, ssize_t* dest_capacity, char* src, ssize_t src_size)
{
    if (!c2b_globals.src_line_str) {
        c2b_globals.src_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
        if (!c2b_globals.src_line_str) {
            fprintf(stderr, "Error: Could not allocate space for globals source line buffer\n");
            exit(ENOMEM);
        }
    }
    if (!c2b_globals.dest_line_str) {
        c2b_globals.dest_line_str = malloc(C2B_MAX_LINE_LENGTH_VALUE + 1);
        if (!c2b_globals.dest_line_str) {
            fprintf(stderr, "Error: Could not allocate space for globals destination line buffer\n");
            exit(ENOMEM);
        }
    }

    /* 
       Initialize and increment parameters
    */

    c2b_globals.wig->line++;
    if (c2b_globals.wig->basename) {
        sprintf(c2b_globals.wig->id,
                "%s.%u",
                c2b_globals.wig->basename,
                c2b_globals.wig->section);
    }

    /* 
       Legal header cases: line starts with '#' 
    */
    
    if (src[0] == c2b_wig_header_prefix) { 
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - c2b_wig_header_prefix - enter ---\n");
#endif
        if (c2b_globals.wig->start_write) {
            c2b_globals.wig->start_write = kFalse;
            sprintf(c2b_globals.wig->id, 
                    "%s.%u",
                    c2b_globals.wig->basename, 
                    ++c2b_globals.wig->section);
        }
        if (c2b_globals.keep_header_flag) {
            /* copy header line to destination stream buffer */
            memcpy(c2b_globals.src_line_str, src, src_size);
            c2b_globals.src_line_str[src_size] = '\0';
            if (!c2b_globals.wig->basename) {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.src_line_str);
            }
            else {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.wig->id,
                        c2b_globals.src_line_str);
            }
            /* resize destination buffer, if necessary */
            if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
                char *dest_line_resized = NULL;
                dest_line_resized = realloc(*dest, *dest_capacity * 2);
                if (dest_line_resized) {
                    *dest_capacity *= 2;
                    *dest = dest_line_resized;
                }
                else {
                    fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                    exit(ENOMEM);
                }
            }
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
            return;
        }
        else {
            return;
        }
    }
    else if ((strncmp(src, c2b_wig_track_prefix, strlen(c2b_wig_track_prefix)) == 0) || 
             (strncmp(src, c2b_wig_browser_prefix, strlen(c2b_wig_browser_prefix)) == 0)) {
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - c2b_wig_track_prefix / c2b_wig_browser_prefix - enter ---\n");
#endif
        if (c2b_globals.wig->start_write) {
            c2b_globals.wig->start_write = kFalse;
            sprintf(c2b_globals.wig->id,
                    "%s.%u",
                    c2b_globals.wig->basename,
                    ++c2b_globals.wig->section);
        }
        if (c2b_globals.keep_header_flag) { 
            /* copy header line to destination stream buffer */
            memcpy(c2b_globals.src_line_str, src, src_size);
            c2b_globals.src_line_str[src_size] = '\0';
            if (!c2b_globals.wig->basename) {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.src_line_str);
            }
            else {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.wig->id,
                        c2b_globals.src_line_str);
            }
            /* resize destination buffer, if necessary */
            if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
                char *dest_line_resized = NULL;
                dest_line_resized = realloc(*dest, *dest_capacity * 2);
                if (dest_line_resized) {
                    *dest_capacity *= 2;
                    *dest = dest_line_resized;
                }
                else {
                    fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                    exit(ENOMEM);
                }
            }
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
            return;
        }
        else {
            return;
        }        
    }
    else if (strncmp(src, c2b_wig_variable_step_prefix, strlen(c2b_wig_variable_step_prefix)) == 0) {
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - c2b_wig_variable_step_prefix - enter ---\n");
#endif
        memcpy(c2b_globals.src_line_str, src, src_size);
        c2b_globals.src_line_str[src_size] = '\0';
        int variable_step_fields = sscanf(c2b_globals.src_line_str, 
                                          "variableStep chrom=%s span=%" SCNu64 "\n", 
                                          c2b_globals.wig->chr, 
                                          &(c2b_globals.wig->span));
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - c2b_wig_variable_step_prefix - variable_step_fields [%d] ---\n", variable_step_fields);
#endif
        if (variable_step_fields < 1) {
            fprintf(stderr, "Error: Invalid variableStep header on line %u\n", c2b_globals.wig->line);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        if (variable_step_fields == 1)
            c2b_globals.wig->span = 1;
        c2b_globals.wig->is_fixed_step = kFalse;
        if (c2b_globals.wig->start_write) {
            c2b_globals.wig->start_write = kFalse;
            sprintf(c2b_globals.wig->id,
                    "%s.%u",
                    c2b_globals.wig->basename,
                    ++c2b_globals.wig->section);
        }
        if (strlen(c2b_globals.wig->id) == 0) {
            fprintf(stderr, "Error: Invalid wig ID on line %u\n", c2b_globals.wig->line);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        if (c2b_globals.keep_header_flag) { 
            /* copy header line to destination stream buffer */
            if (!c2b_globals.wig->basename) {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.src_line_str);
            }
            else {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.wig->id,
                        c2b_globals.src_line_str);
            }
            /* resize destination buffer, if necessary */
            if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
                char *dest_line_resized = NULL;
                dest_line_resized = realloc(*dest, *dest_capacity * 2);
                if (dest_line_resized) {
                    *dest_capacity *= 2;
                    *dest = dest_line_resized;
                }
                else {
                    fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                    exit(ENOMEM);
                }
            }
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
            return;
        }
        else {
            return;
        }                
    }
    else if (strncmp(src, c2b_wig_fixed_step_prefix, strlen(c2b_wig_fixed_step_prefix)) == 0) {
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - c2b_wig_fixed_step_prefix - enter ---\n");
#endif
        memcpy(c2b_globals.src_line_str, src, src_size);
        c2b_globals.src_line_str[src_size] = '\0';
        int fixed_step_fields = sscanf(c2b_globals.src_line_str, 
                                       "fixedStep chrom=%s start=%" SCNu64 " step=%" SCNu64 " span=%" SCNu64 "\n", 
                                       c2b_globals.wig->chr, 
                                       &(c2b_globals.wig->start_pos), 
                                       &(c2b_globals.wig->step), 
                                       &(c2b_globals.wig->span));
        if (fixed_step_fields < 3) {
            fprintf(stderr, "Error: Invalid fixedStep header on line %u\n", c2b_globals.wig->line);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        if (strlen(c2b_globals.wig->chr) >= C2B_MAX_CHROMOSOME_LENGTH) {
            fprintf(stderr, "Error: Invalid WIG line %u\n", c2b_globals.wig->line);
            fprintf(stderr, "Chromosome length is too long; check that you have Unix newlines (cat -A)\n" \
                            "or increase TOKEN_CHR_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS\n");
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        if (fixed_step_fields == 3) {
            c2b_globals.wig->span = 1;
        }
        c2b_globals.wig->is_fixed_step = kTrue;
        if (c2b_globals.keep_header_flag) { 
            /* copy header line to destination stream buffer */
            if (!c2b_globals.wig->basename) {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.src_line_str);
            }
            else {
                sprintf(c2b_globals.dest_line_str, 
                        "%s\t%u\t%u\t%s\t%s\n", 
                        c2b_header_chr_name, 
                        c2b_globals.header_line_idx, 
                        c2b_globals.header_line_idx + 1, 
                        c2b_globals.wig->id,
                        c2b_globals.src_line_str);
            }
            /* resize destination buffer, if necessary */
            if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
                char *dest_line_resized = NULL;
                dest_line_resized = realloc(*dest, *dest_capacity * 2);
                if (dest_line_resized) {
                    *dest_capacity *= 2;
                    *dest = dest_line_resized;
                }
                else {
                    fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                    exit(ENOMEM);
                }
            }
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
            c2b_globals.header_line_idx++;
            return;
        }
        else {
            return;
        }        
    }
    else if (strncmp(src, c2b_wig_chr_prefix, strlen(c2b_wig_chr_prefix)) == 0) {
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - c2b_wig_chr_prefix - enter ---\n");
#endif
        memcpy(c2b_globals.src_line_str, src, src_size);
        c2b_globals.src_line_str[src_size] = '\0';
        int bed_fields = sscanf(c2b_globals.src_line_str, 
                                "%s\t%" SCNu64 "\t%" SCNu64 "\t%lf\n", 
                                c2b_globals.wig->chr,
                                &(c2b_globals.wig->start_pos), 
                                &(c2b_globals.wig->end_pos), 
                                &(c2b_globals.wig->score));
        if (bed_fields != 4) {
            fprintf(stderr, "Error: Invalid WIG line %u\n", c2b_globals.wig->line);
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        if (strlen(c2b_globals.wig->chr) >= C2B_MAX_CHROMOSOME_LENGTH) {
            fprintf(stderr, "Error: Invalid WIG line %u\n", c2b_globals.wig->line);
            fprintf(stderr, "Chromosome length is too long; check that you have Unix newlines (cat -A)\n" \
                            "or increase TOKEN_CHR_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS\n");
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        c2b_globals.wig->pos_lines++;
        if ((c2b_globals.wig->start_pos == 0) && (!c2b_globals.zero_indexed_flag)) {
            fprintf(stderr, "Error: WIG data contains 0-indexed element at line %u\n", c2b_globals.wig->line);
            fprintf(stderr, "       Consider adding --zero-indexed (-x) option to convert zero-indexed WIG data\n");
            exit(EINVAL); /* Invalid argument (POSIX.1) */
        }
        if (!c2b_globals.wig->basename) {
            sprintf(c2b_globals.dest_line_str,
                    "%s\t"                      \
                    "%" PRIu64 "\t"             \
                    "%" PRIu64 "\t"             \
                    "id-%d\t"                   \
                    "%lf\n",
                    c2b_globals.wig->chr,
                    c2b_globals.wig->start_pos - c2b_globals.wig->start_shift,
                    c2b_globals.wig->end_pos - c2b_globals.wig->end_shift,
                    c2b_globals.wig->pos_lines,
                    c2b_globals.wig->score);
        }
        else {
            sprintf(c2b_globals.dest_line_str,
                    "%s\t"                      \
                    "%" PRIu64 "\t"             \
                    "%" PRIu64 "\t"             \
                    "%s-%d\t"                   \
                    "%lf\n",
                    c2b_globals.wig->chr,
                    c2b_globals.wig->start_pos - c2b_globals.wig->start_shift,
                    c2b_globals.wig->end_pos - c2b_globals.wig->end_shift,
                    c2b_globals.wig->id,
                    c2b_globals.wig->pos_lines,
                    c2b_globals.wig->score);
        }
        c2b_globals.wig->start_write = kTrue;
        /* resize destination buffer, if necessary */
        if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
            char *dest_line_resized = NULL;
            dest_line_resized = realloc(*dest, *dest_capacity * 2);
            if (dest_line_resized) {
                *dest_capacity *= 2;
                *dest = dest_line_resized;
            }
            else {
                fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                exit(ENOMEM);
            }
        }
        memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
        *dest_size += strlen(c2b_globals.dest_line_str);
    }
    else {
#ifdef DEBUG
        fprintf (stderr, "--- convert2bed c2b_line_convert_wig_to_bed_unsorted() - src_line_str - enter ---\n");
#endif
        memcpy(c2b_globals.src_line_str, src, src_size);
        c2b_globals.src_line_str[src_size] = '\0';

        if (c2b_globals.wig->is_fixed_step) {

            int fixed_step_column_fields = sscanf(c2b_globals.src_line_str, "%lf\n", &(c2b_globals.wig->score));
            if (fixed_step_column_fields != 1) {
                fprintf(stderr, "Error: Invalid WIG line %u\n", c2b_globals.wig->line);
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            c2b_globals.wig->pos_lines++;
            if ((c2b_globals.wig->start_pos == 0) && (!c2b_globals.zero_indexed_flag)) {
                fprintf(stderr, "Error: WIG data contains 0-indexed element at line %u\n", c2b_globals.wig->line);
                fprintf(stderr, "       Consider adding --zero-indexed (-x) option to convert zero-indexed WIG data\n");
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            if (!c2b_globals.wig->basename) {
                sprintf(c2b_globals.dest_line_str,
                        "%s\t"                  \
                        "%" PRIu64 "\t"         \
                        "%" PRIu64 "\t"         \
                        "id-%d\t"               \
                        "%lf\n",
                        c2b_globals.wig->chr,
                        c2b_globals.wig->start_pos - c2b_globals.wig->start_shift,
                        c2b_globals.wig->start_pos + c2b_globals.wig->span - c2b_globals.wig->end_shift,
                        c2b_globals.wig->pos_lines,
                        c2b_globals.wig->score);
            }
            else {
                sprintf(c2b_globals.dest_line_str,
                        "%s\t"                  \
                        "%" PRIu64 "\t"         \
                        "%" PRIu64 "\t"         \
                        "%s-%d\t"               \
                        "%lf\n",
                        c2b_globals.wig->chr,
                        c2b_globals.wig->start_pos - c2b_globals.wig->start_shift,
                        c2b_globals.wig->start_pos + c2b_globals.wig->span - c2b_globals.wig->end_shift,
                        c2b_globals.wig->id,
                        c2b_globals.wig->pos_lines,
                        c2b_globals.wig->score);
            }            
            c2b_globals.wig->start_pos += c2b_globals.wig->step;
            c2b_globals.wig->start_write = kTrue;
            /* resize destination buffer, if necessary */
            if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
                char *dest_line_resized = NULL;
                dest_line_resized = realloc(*dest, *dest_capacity * 2);
                if (dest_line_resized) {
                    *dest_capacity *= 2;
                    *dest = dest_line_resized;
                }
                else {
                    fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                    exit(ENOMEM);
                }
            }
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
        }
        else {
            int variable_step_column_fields = sscanf(c2b_globals.src_line_str, 
                                                     "%" SCNu64 "\t%lf\n", 
                                                     &(c2b_globals.wig->start_pos), 
                                                     &(c2b_globals.wig->score));
            if (variable_step_column_fields != 2) {
                fprintf(stderr, "Error: Invalid WIG line %u\n", c2b_globals.wig->line);
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            c2b_globals.wig->pos_lines++;
            if ((c2b_globals.wig->start_pos == 0) && (!c2b_globals.zero_indexed_flag)) {
                fprintf(stderr, "Error: WIG data contains 0-indexed element at line %u\n", c2b_globals.wig->line);
                fprintf(stderr, "       Consider adding --zero-indexed (-x) option to convert zero-indexed WIG data\n");
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            if (!c2b_globals.wig->basename) {
                sprintf(c2b_globals.dest_line_str,
                        "%s\t"                  \
                        "%" PRIu64 "\t"         \
                        "%" PRIu64 "\t"         \
                        "id-%d\t"               \
                        "%lf\n",
                        c2b_globals.wig->chr,
                        c2b_globals.wig->start_pos - c2b_globals.wig->start_shift,
                        c2b_globals.wig->start_pos + c2b_globals.wig->span - c2b_globals.wig->end_shift,
                        c2b_globals.wig->pos_lines,
                        c2b_globals.wig->score);
            }
            else {
                sprintf(c2b_globals.dest_line_str,
                        "%s\t"                  \
                        "%" PRIu64 "\t"         \
                        "%" PRIu64 "\t"         \
                        "%s-%d\t"               \
                        "%lf\n",
                        c2b_globals.wig->chr,
                        c2b_globals.wig->start_pos - c2b_globals.wig->start_shift,
                        c2b_globals.wig->start_pos + c2b_globals.wig->span - c2b_globals.wig->end_shift,
                        c2b_globals.wig->id,
                        c2b_globals.wig->pos_lines,
                        c2b_globals.wig->score);
            }            
            c2b_globals.wig->start_pos += c2b_globals.wig->step;
            c2b_globals.wig->start_write = kTrue;
            /* resize destination buffer, if necessary */
            if ((ssize_t)(*dest_size + C2B_MAX_LINE_LENGTH_VALUE) > *dest_capacity) {
                char *dest_line_resized = NULL;
                dest_line_resized = realloc(*dest, *dest_capacity * 2);
                if (dest_line_resized) {
                    *dest_capacity *= 2;
                    *dest = dest_line_resized;
                }
                else {
                    fprintf(stderr, "Error: Could not resize dest string in WIG pointer conversion fn\n");
                    exit(ENOMEM);
                }
            }
            memcpy(*dest + *dest_size, c2b_globals.dest_line_str, strlen(c2b_globals.dest_line_str));
            *dest_size += strlen(c2b_globals.dest_line_str);
        }
    }
}

static void *
c2b_read_bytes_from_stdin(void* arg)
{
    c2b_pipeline_stage_t* stage = (c2b_pipeline_stage_t *) arg;
    c2b_pipeset_t* pipes = stage->pipeset;
    char* buffer = NULL;
    ssize_t buffer_size = stage->buffer_size;
    ssize_t bytes_read;
    int exit_status = 0;

#ifdef DEBUG
    fprintf(stderr, "\t-> c2b_read_bytes_from_stdin | reading from fd     (%02d) | writing to fd     (%02d)\n", STDIN_FILENO, pipes->in[stage->dest][PIPE_WRITE]);
#endif

    buffer = (char *) malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Error: Could not allocate space to c2b_read_bytes_from_stdin() buffer\n");
        exit(ENOMEM);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    while ((bytes_read = read(STDIN_FILENO, buffer, buffer_size)) > 0) {
        write(pipes->in[stage->dest][PIPE_WRITE], buffer, bytes_read);
    }
#pragma GCC diagnostic pop
    close(pipes->in[stage->dest][PIPE_WRITE]);

    free(buffer), buffer = NULL;

    if (WIFEXITED(stage->status) || WIFSIGNALED(stage->status)) {
        waitpid(stage->pid, &stage->status, WUNTRACED);
        exit_status = WEXITSTATUS(stage->status);
        if (exit_status != 0) 
            fprintf(stderr, 
                    "Error: Stage [%s] failed -- exit status [%d | %d]\n", 
                    stage->description,
                    stage->status, 
                    exit_status);
    }

    pthread_exit(NULL);
}

static void *
c2b_process_intermediate_bytes_by_lines(void* arg)
{
    c2b_pipeline_stage_t* stage = (c2b_pipeline_stage_t *) arg;
    c2b_pipeset_t* pipes = stage->pipeset;
    char* src_buffer = NULL;
    ssize_t src_buffer_capacity = C2B_THREAD_IO_BUFFER_SIZE;
    ssize_t src_bytes_read = 0;
    ssize_t remainder_length = 0;
    ssize_t remainder_offset = 0;
    char line_delim = '\n';
    ssize_t lines_offset = 0;
    ssize_t start_offset = 0;
    ssize_t end_offset = 0;
    char* dest_buffer = NULL;
    ssize_t dest_buffer_capacity = C2B_THREAD_IO_BUFFER_SIZE;
    ssize_t dest_bytes_written = 0;
    void (*line_functor)(char **, ssize_t *, ssize_t *, char *, ssize_t) = stage->line_functor;
    int exit_status = 0;

    /* 
       We read from the src out pipe, then write to the dest in pipe 
    */
    
    src_buffer = malloc(src_buffer_capacity);
    if (!src_buffer) {
        fprintf(stderr, "Error: Could not allocate space for intermediate source buffer.\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    dest_buffer = malloc(dest_buffer_capacity);
    if (!dest_buffer) {
        fprintf(stderr, "Error: Could not allocate space for intermediate destination buffer.\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    dest_buffer[0] = '\0';

    while ((src_bytes_read = read(pipes->out[stage->src][PIPE_READ],
                                  src_buffer + remainder_length,
                                  src_buffer_capacity - remainder_length)) > 0) {

        /* 
           So here's what src_buffer looks like initially; basically, some stuff separated by
           newlines. The src_buffer will probably not terminate with a newline. So we first use 
           a custom memrchr() call to find the remainder_offset index value:
           
           src_buffer  [  .  .  .  \n  .  .  .  \n  .  .  .  \n  .  .  .  .  .  .  ]
           index        0 1 2 ...                            ^                    ^
                                                             |                    |
                                                             |   src_bytes_read --
                                                             | 
                                         remainder_offset --- 

           In other words, everything at and after index [remainder_offset + 1] to index
           [src_bytes_read - 1] is a remainder byte ("R"):

           src_buffer  [  .  .  .  \n  .  .  .  \n  .  .  .  \n R R R R R ]
           
           Assuming this failed:

           If remainder_offset is -1 and we have read src_buffer_size bytes, then we know there 
           are no newlines anywhere in the src_buffer and so we terminate early with an error state.
           This would suggest either src_buffer_size is too small to hold a whole intermediate 
           line or the input data is perhaps corrupt. Basically, something went wrong and we need 
           to investigate.
           
           Asumming this worked:
           
           We can now parse byte indices {[0 .. remainder_offset]} into lines, which are fed one
           by one to the line_functor. This functor parses out tab offsets and writes out a 
           reordered string based on the rules for the format (see BEDOPS docs for reordering 
           table).
           
           Finally, we write bytes from index [remainder_offset + 1] to [src_bytes_read - 1] 
           back to src_buffer. We are writing remainder_length bytes:
           
           new_remainder_length = current_src_bytes_read + old_remainder_length - new_remainder_offset
           
           Note that we can leave the rest of the buffer untouched:
           
           src_buffer  [ R R R R R \n  .  .  .  \n  .  .  .  \n  .  .  .  ]
           
           On the subsequent read, we want to read() into src_buffer at position remainder_length
           and read up to, at most, (src_buffer_size - remainder_length) bytes:
           
           read(byte_source, 
                src_buffer + remainder_length,
                src_buffer_size - remainder_length)

           Second and subsequent reads should reduce the maximum number of src_bytes_read from 
           src_buffer_size to something smaller.
        */

        c2b_memrchr_offset(&remainder_offset, src_buffer, src_buffer_capacity, src_bytes_read + remainder_length, line_delim);

        if (remainder_offset == -1) {
            if (src_bytes_read + remainder_length == src_buffer_capacity) {
                fprintf(stderr, "Error: Could not find newline in intermediate buffer; check input [%zu | %zu | %zu]\n", (unsigned long) src_bytes_read, (unsigned long) remainder_length, (unsigned long) src_buffer_capacity);
                fprintf(stderr, "       Please check that your input contains Unix newlines (cat -A) or increase TOKENS_MAX_LENGTH in BEDOPS.Constants.hpp and recompile BEDOPS.\n");
                c2b_print_usage(stderr);
                exit(EINVAL); /* Invalid argument (POSIX.1) */
            }
            remainder_offset = 0;
        }

        /* 
           We next want to process bytes from index [0] to index [remainder_offset - 1] for all
           lines contained within. We basically build a buffer containing all translated 
           lines to write downstream.
        */

        lines_offset = 0;
        start_offset = 0;
        dest_bytes_written = 0;
        while (lines_offset < remainder_offset) {
            if (src_buffer[lines_offset] == line_delim) {
                end_offset = lines_offset;
                /* for a given line from src, we write dest_bytes_written number of bytes to dest_buffer (plus written offset) */
                (*line_functor)(&dest_buffer, &dest_bytes_written, &dest_buffer_capacity, src_buffer + start_offset, end_offset - start_offset);
                start_offset = end_offset + 1;
            }
            lines_offset++;            
        }
        
        /* 
           We have filled up dest_buffer with translated bytes (dest_bytes_written of them)
           and can now write() this buffer to the in-pipe of the destination stage
        */
        
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        write(pipes->in[stage->dest][PIPE_WRITE], dest_buffer, dest_bytes_written);
        //fprintf(stderr, "dest_bytes_written [%zu] of dest_buffer_size [%zu] (diff [%zu])\n", dest_bytes_written, dest_buffer_capacity, dest_buffer_capacity - dest_bytes_written);
#pragma GCC diagnostic pop

        remainder_length = src_bytes_read + remainder_length - remainder_offset;
        memcpy(src_buffer, src_buffer + remainder_offset, remainder_length);
    }

    close(pipes->in[stage->dest][PIPE_WRITE]);

    if (src_buffer) 
        free(src_buffer), src_buffer = NULL;

    if (dest_buffer)
        free(dest_buffer), dest_buffer = NULL;

    if (WIFEXITED(stage->status) || WIFSIGNALED(stage->status)) {
        waitpid(stage->pid, &stage->status, WUNTRACED);
        exit_status = WEXITSTATUS(stage->status);
        if (exit_status != 0) 
            fprintf(stderr, 
                    "Error: Stage [%s] failed -- exit status [%d | %d]\n", 
                    stage->description,
                    stage->status, 
                    exit_status);
    }

    pthread_exit(NULL);
}

static void *
c2b_write_in_bytes_to_in_process(void* arg)
{
    c2b_pipeline_stage_t* stage = (c2b_pipeline_stage_t *) arg;
    c2b_pipeset_t* pipes = stage->pipeset;
    char* buffer = NULL;
    ssize_t buffer_size = stage->buffer_size;
    ssize_t bytes_read;
    int exit_status = 0;

    buffer = (char *) malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Error: Could not allocate space to c2b_write_in_bytes_to_in_process() buffer\n");
        exit(ENOMEM);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    /* read buffer from p->in[1] and write buffer to p->in[2] */
    while ((bytes_read = read(pipes->in[stage->src][PIPE_READ], buffer, buffer_size)) > 0) { 
        write(pipes->in[stage->dest][PIPE_WRITE], buffer, bytes_read);
    }
#pragma GCC diagnostic pop

    close(pipes->in[stage->dest][PIPE_WRITE]);

    free(buffer), buffer = NULL;

    if (WIFEXITED(stage->status) || WIFSIGNALED(stage->status)) {
        waitpid(stage->pid, &stage->status, WUNTRACED);
        exit_status = WEXITSTATUS(stage->status);
        if (exit_status != 0) 
            fprintf(stderr, 
                    "Error: Stage [%s] failed -- exit status [%d | %d]\n", 
                    stage->description,
                    stage->status, 
                    exit_status);
    }

    pthread_exit(NULL);
}

static void *
c2b_write_out_bytes_to_in_process(void* arg)
{
    c2b_pipeline_stage_t* stage = (c2b_pipeline_stage_t *) arg;
    c2b_pipeset_t* pipes = stage->pipeset;
    char* buffer = NULL;
    ssize_t buffer_size = stage->buffer_size;
    ssize_t bytes_read;
    int exit_status = 0;

    buffer = (char *) malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Error: Could not allocate space to c2b_write_out_bytes_to_in_process() buffer\n");
        exit(ENOMEM);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    /* read buffer from p->out[1] and write buffer to p->in[2] */
    while ((bytes_read = read(pipes->out[stage->src][PIPE_READ], buffer, buffer_size)) > 0) { 
        write(pipes->in[stage->dest][PIPE_WRITE], buffer, bytes_read);
    }
#pragma GCC diagnostic pop

    close(pipes->in[stage->dest][PIPE_WRITE]);

    free(buffer), buffer = NULL;

    if (WIFEXITED(stage->status) || WIFSIGNALED(stage->status)) {
        waitpid(stage->pid, &stage->status, WUNTRACED);
        exit_status = WEXITSTATUS(stage->status);
        if (exit_status != 0) 
            fprintf(stderr, 
                    "Error: Stage [%s] failed -- exit status [%d | %d]\n", 
                    stage->description,
                    stage->status, 
                    exit_status);
    }

    pthread_exit(NULL);
}

static void *
c2b_write_in_bytes_to_stdout(void* arg)
{
    c2b_pipeline_stage_t* stage = (c2b_pipeline_stage_t *) arg;
    c2b_pipeset_t* pipes = stage->pipeset;
    char* buffer = NULL;
    ssize_t buffer_size = stage->buffer_size;
    ssize_t bytes_read;
    int exit_status = 0;

    buffer = (char *) malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Error: Could not allocate space to c2b_write_in_bytes_to_stdout() buffer\n");
        exit(ENOMEM);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    while ((bytes_read = read(pipes->in[stage->src][PIPE_READ], buffer, buffer_size)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
#pragma GCC diagnostic pop

    free(buffer), buffer = NULL;

    if (WIFEXITED(stage->status) || WIFSIGNALED(stage->status)) {
        waitpid(stage->pid, &stage->status, WUNTRACED);
        exit_status = WEXITSTATUS(stage->status);
        if (exit_status != 0) 
            fprintf(stderr, 
                    "Error: Stage [%s] failed -- exit status [%d | %d]\n", 
                    stage->description,
                    stage->status, 
                    exit_status);
    }

    pthread_exit(NULL);
}

static void *
c2b_write_out_bytes_to_stdout(void* arg)
{
    c2b_pipeline_stage_t* stage = (c2b_pipeline_stage_t *) arg;
    c2b_pipeset_t* pipes = stage->pipeset;
    char* buffer = NULL;
    ssize_t buffer_size = stage->buffer_size;
    ssize_t bytes_read;
    int exit_status = 0;

    buffer = (char *) malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Error: Could not allocate space to c2b_write_out_bytes_to_stdout() buffer\n");
        exit(ENOMEM);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    while ((bytes_read = read(pipes->out[stage->src][PIPE_READ], buffer, buffer_size)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
#pragma GCC diagnostic pop

    free(buffer), buffer = NULL;

    if (WIFEXITED(stage->status) || WIFSIGNALED(stage->status)) {
        waitpid(stage->pid, &stage->status, WUNTRACED);
        exit_status = WEXITSTATUS(stage->status);
        if (exit_status != 0) 
            fprintf(stderr, 
                    "Error: Stage [%s] failed -- exit status [%d | %d]\n", 
                    stage->description,
                    stage->status, 
                    exit_status);
    }

    pthread_exit(NULL);
}

static void
c2b_memrchr_offset(ssize_t* offset, char* buf, ssize_t buf_size, ssize_t len, char delim)
{
    ssize_t left = len;
    
    *offset = -1;
    if (len > buf_size)
        return;

    while (left > 0) {
        if (buf[left - 1] == delim) {
            *offset = left;
            return;
        }
        left--;
    }
}

static void
c2b_init_pipeset(c2b_pipeset_t* p, const size_t num)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_pipeset() - enter ---\n");
#endif

    int** ins = NULL;
    int** outs = NULL;
    int** errs = NULL;
    size_t n;

    ins = malloc(num * sizeof(int *));
    outs = malloc(num * sizeof(int *));
    errs = malloc(num * sizeof(int *));
    if ((!ins) || (!outs) || (!errs)) {
        fprintf(stderr, "Error: Could not allocate space to temporary pipe arrays\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    p->in = ins;
    p->out = outs;
    p->err = errs;

    for (n = 0; n < num; n++) {
    p->in[n] = NULL;
    p->in[n] = malloc(PIPE_STREAMS * sizeof(int));
    if (!p->in[n]) {
        fprintf(stderr, "Error: Could not allocate space to temporary internal pipe array\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    p->out[n] = NULL;
    p->out[n] = malloc(PIPE_STREAMS * sizeof(int));
    if (!p->out[n]) {
        fprintf(stderr, "Error: Could not allocate space to temporary internal pipe array\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    p->err[n] = NULL;
    p->err[n] = malloc(PIPE_STREAMS * sizeof(int));
    if (!p->err[n]) {
        fprintf(stderr, "Error: Could not allocate space to temporary internal pipe array\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    /* set close-on-exec flag for each pipe */
    c2b_pipe4_cloexec(p->in[n]);
    c2b_pipe4_cloexec(p->out[n]);
    c2b_pipe4_cloexec(p->err[n]);

        /* set stderr as output for each err write */
        p->err[n][PIPE_WRITE] = STDERR_FILENO;
    }

    p->num = num;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_pipeset() - exit  ---\n");
#endif
}

/* 
   specifying special attribute for c2b_debug_pipeset() to avoid: "warning: unused 
   function 'c2b_debug_pipeset' [-Wunused-function]" message during non-debug compilation

   cf. http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Function-Attributes.html#Function%20Attributes
*/
#if defined(__GNUC__)
static void c2b_debug_pipeset() __attribute__ ((unused));
#endif

static void
c2b_debug_pipeset(c2b_pipeset_t* p, const size_t num)
{
    size_t n;
    size_t s;

    for (n = 0; n < num; n++) {
        for (s = 0; s < PIPE_STREAMS; s++) {
            fprintf(stderr, "\t-> c2b_debug_pipeset - [n, s] = [%zu, %zu] - [%s\t- in, out, err] = [%d, %d, %d]\n",
                    n, 
                    s, 
                    (!s ? "READ" : "WRITE"),
                    p->in[n][s],
                    p->out[n][s],
                    p->err[n][s]);
        }
    }
}

static void
c2b_delete_pipeset(c2b_pipeset_t* p)
{
    size_t n;
    size_t s;

    for (n = 0; n < p->num; n++) {
        for (s = 0; s < PIPE_STREAMS; s++) {
            close(p->in[n][s]);
            close(p->out[n][s]);
            close(p->err[n][s]);
        }
        free(p->in[n]), p->in[n] = NULL;
        free(p->out[n]), p->out[n] = NULL;
        free(p->err[n]), p->err[n] = NULL;
    }
    free(p->in), p->in = NULL;
    free(p->out), p->out = NULL;
    free(p->err), p->err = NULL;

    p->num = 0;
}

static void
c2b_set_close_exec_flag(int fd)
{
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
}

static void
c2b_unset_close_exec_flag(int fd)
{
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) & ~FD_CLOEXEC);
}

static int
c2b_pipe4(int fd[2], int flags)
{
    int ret = pipe(fd);
    if (flags & PIPE4_FLAG_RD_CLOEXEC) {
        c2b_set_close_exec_flag(fd[PIPE_READ]);
    }
    if (flags & PIPE4_FLAG_WR_CLOEXEC) {
        c2b_set_close_exec_flag(fd[PIPE_WRITE]);
    }
    return ret;
}

static pid_t
c2b_popen4(const char* cmd, int pin[2], int pout[2], int perr[2], int flags)
{
    pid_t ret = fork();
    int execl_ret = 0;

    if (ret < 0) {
        fprintf(stderr, "fork() failed!\n");
        return ret;
    } 
    else if (ret == 0) {
        /* 
           Child-side of fork
        */
        if (flags & POPEN4_FLAG_CLOSE_CHILD_STDIN) {
            close(STDIN_FILENO);
        }
        else {
            c2b_unset_close_exec_flag(pin[PIPE_READ]);
            dup2(pin[PIPE_READ], STDIN_FILENO);
        }
        if (flags & POPEN4_FLAG_CLOSE_CHILD_STDOUT) {
            close(STDOUT_FILENO);
        }
        else {
            c2b_unset_close_exec_flag(pout[PIPE_WRITE]);
            dup2(pout[PIPE_WRITE], STDOUT_FILENO);
        }
        if (flags & POPEN4_FLAG_CLOSE_CHILD_STDERR) {
            close(STDERR_FILENO);
        }
        else {
            c2b_unset_close_exec_flag(perr[PIPE_WRITE]);
            dup2(perr[PIPE_WRITE], STDERR_FILENO);
        }
        execl_ret = execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
        if (execl_ret == -1) {
            int errsv = errno;
            fprintf(stderr, "Error: exec() failed (%d)\n", errsv);
            exit(errsv);
        }
    }
    else {
        /* 
           Parent-side of fork
        */
        if (~flags & POPEN4_FLAG_NOCLOSE_PARENT_STDIN && ~flags & POPEN4_FLAG_CLOSE_CHILD_STDIN) {
            close(pin[PIPE_READ]);
        }
        if (~flags & POPEN4_FLAG_NOCLOSE_PARENT_STDOUT && ~flags & POPEN4_FLAG_CLOSE_CHILD_STDOUT) {
            close(pout[PIPE_WRITE]);
        }
        if (~flags & POPEN4_FLAG_NOCLOSE_PARENT_STDERR && ~flags & POPEN4_FLAG_CLOSE_CHILD_STDERR) {
            //close(perr[PIPE_WRITE]);
        }
        return ret;
    }

    /* Unreachable */
    return ret;
}

static void
c2b_test_dependencies()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_test_dependencies() - enter ---\n");
#endif

    char* p = NULL;
    char* path = NULL;

    if ((p = getenv("PATH")) == NULL) {
        fprintf(stderr, "Error: Cannot retrieve environment PATH variable\n");
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }
    path = malloc(strlen(p) + 1);
    if (!path) {
        fprintf(stderr, "Error: Cannot allocate space for path variable copy\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    memcpy(path, p, strlen(p) + 1);

    if ((c2b_globals.input_format_idx == BAM_FORMAT) || (c2b_globals.input_format_idx == SAM_FORMAT)) {
        char* samtools = NULL;
        samtools = malloc(strlen(c2b_samtools) + 1);
        if (!samtools) {
            fprintf(stderr, "Error: Cannot allocate space for samtools variable copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(samtools, c2b_samtools, strlen(c2b_samtools) + 1);

        char* path_samtools = NULL;
        path_samtools = malloc(strlen(path) + 1);
        if (!path_samtools) {
            fprintf(stderr, "Error: Cannot allocate space for path (samtools) copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(path_samtools, path, strlen(path) + 1);

#ifdef DEBUG
        fprintf(stderr, "Debug: Searching [%s] for samtools\n", path_samtools);
#endif

        if (c2b_print_matches(path_samtools, samtools) != kTrue) {
            fprintf(stderr, "Error: Cannot find samtools binary required for conversion of BAM and SAM format\n");
            c2b_print_usage(stderr);
            exit(ENOENT); /* No such file or directory (POSIX.1) */
        }
        free(path_samtools), path_samtools = NULL;
        free(samtools), samtools = NULL;
    }

    if (c2b_globals.sort->is_enabled) {
        char* sort_bed = NULL;
        sort_bed = malloc(strlen(c2b_sort_bed) + 1);
        if (!sort_bed) {
            fprintf(stderr, "Error: Cannot allocate space for sort-bed variable copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(sort_bed, c2b_sort_bed, strlen(c2b_sort_bed) + 1);

        char* path_sort_bed = NULL;
        path_sort_bed = malloc(strlen(path) + 1);
        if (!path_sort_bed) {
            fprintf(stderr, "Error: Cannot allocate space for path (samtools) copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(path_sort_bed, path, strlen(path) + 1);

#ifdef DEBUG
        fprintf(stderr, "Debug: Searching [%s] for sort-bed\n", path_sort_bed);
#endif

        if (c2b_print_matches(path_sort_bed, sort_bed) != kTrue) {
            fprintf(stderr, "Error: Cannot find sort-bed binary required for sorting BED output\n");
            c2b_print_usage(stderr);
            exit(ENOENT); /* No such file or directory (POSIX.1) */
        }
        free(path_sort_bed), path_sort_bed = NULL;
        free(sort_bed), sort_bed = NULL;
    }

    if (c2b_globals.output_format_idx == STARCH_FORMAT) {
        char* starch = NULL;
        starch = malloc(strlen(c2b_starch) + 1);
        if (!starch) {
            fprintf(stderr, "Error: Cannot allocate space for starch variable copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(starch, c2b_starch, strlen(c2b_starch) + 1);

        char* path_starch = NULL;
        path_starch = malloc(strlen(path) + 1);
        if (!path_starch) {
            fprintf(stderr, "Error: Cannot allocate space for path (starch) copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(path_starch, path, strlen(path) + 1);

#ifdef DEBUG
        fprintf(stderr, "Debug: Searching [%s] for starch\n", path_starch);
#endif

        if (c2b_print_matches(path_starch, starch) != kTrue) {
            fprintf(stderr, "Error: Cannot find starch binary required for compression of BED output\n");
            c2b_print_usage(stderr);
            exit(ENOENT); /* No such file or directory (POSIX.1) */
        }
        free(path_starch), path_starch = NULL;
        free(starch), starch = NULL;
    }

    if (c2b_globals.input_format_idx != BAM_FORMAT) {
        char* cat = NULL;
        cat = malloc(strlen(c2b_cat) + 1);
        if (!cat) {
            fprintf(stderr, "Error: Cannot allocate space for cat variable copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(cat, c2b_cat, strlen(c2b_cat) + 1);
        
        char* path_cat = NULL;
        path_cat = malloc(strlen(path) + 1);
        if (!path_cat) {
            fprintf(stderr, "Error: Cannot allocate space for path (cat) copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(path_cat, path, strlen(path) + 1);
        
#ifdef DEBUG
        fprintf(stderr, "Debug: Searching [%s] for cat\n", path_cat);
#endif
        
        if (c2b_print_matches(path_cat, cat) != kTrue) {
            fprintf(stderr, "Error: Cannot find cat binary required for piping IO\n");
            c2b_print_usage(stderr);
            exit(ENOENT); /* No such file or directory (POSIX.1) */
        }
        free(path_cat), path_cat = NULL;
        free(cat), cat = NULL;
    }

    free(path), path = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_test_dependencies() - exit  ---\n");
#endif
}

static boolean
c2b_print_matches(char* path, char* fn)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_matches() - enter ---\n");
#endif

    char candidate[PATH_MAX];
    const char* d;
    boolean found = kFalse;

    if (strchr(fn, '/') != NULL) {
        return (c2b_is_there(fn) ? kTrue : kFalse);
    }
    while ((d = c2b_strsep(&path, ":")) != NULL) {
        if (*d == '\0') {
            d = ".";
        }
        if (snprintf(candidate, sizeof(candidate), "%s/%s", d, fn) >= (int) sizeof(candidate)) {
            continue;
        }
        if (c2b_is_there(candidate)) {
            found = kTrue;
            if (strcmp(fn, c2b_samtools) == 0) {
                c2b_globals.sam->samtools_path = malloc(strlen(candidate) + 1);
                if (!c2b_globals.sam->samtools_path) {
                    fprintf(stderr, "Error: Could not allocate space for storing samtools path global\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.sam->samtools_path, candidate, strlen(candidate));
                c2b_globals.sam->samtools_path[strlen(candidate)] = '\0';
            }
            else if (strcmp(fn, c2b_sort_bed) == 0) {
                c2b_globals.sort->sort_bed_path = malloc(strlen(candidate) + 1);
                if (!c2b_globals.sort->sort_bed_path) {
                    fprintf(stderr, "Error: Could not allocate space for storing sortbed path global\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.sort->sort_bed_path, candidate, strlen(candidate));
                c2b_globals.sort->sort_bed_path[strlen(candidate)] = '\0';
            }
            else if (strcmp(fn, c2b_starch) == 0) {
                c2b_globals.starch->path = malloc(strlen(candidate) + 1);
                if (!c2b_globals.starch->path) {
                    fprintf(stderr, "Error: Could not allocate space for storing starch path global\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.starch->path, candidate, strlen(candidate));
                c2b_globals.starch->path[strlen(candidate)] = '\0';
            }
            else if (strcmp(fn, c2b_cat) == 0) {
                c2b_globals.cat->path = malloc(strlen(candidate) + 1);
                if (!c2b_globals.cat->path) {
                    fprintf(stderr, "Errrpr: Could not allocate space for storing cat path global\n");
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.cat->path, candidate, strlen(candidate));
                c2b_globals.cat->path[strlen(candidate)] = '\0';
            }
            break;
        }
    }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_matches() - exit  ---\n");
#endif

    return found;
}

static char *
c2b_strsep(char** stringp, const char* delim)
{
    char* s;
    const char* spanp;
    int c, sc;
    char* tok;

    if ((s = *stringp) == NULL)
        return NULL;

    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return tok;
            }
        } while (sc != 0);
    }

    return NULL;
}

static boolean
c2b_is_there(char* candidate)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_is_there() - enter ---\n");
#endif

    struct stat fin;
    boolean found = kFalse;

    if (access(candidate, X_OK) == 0 
        && stat(candidate, &fin) == 0 
        && S_ISREG(fin.st_mode) 
        && (getuid() != 0 || (fin.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0)) {
#ifdef DEBUG
        fprintf(stderr, "Debug: Found dependency [%s]\n", candidate);
#endif
        found = kTrue;
    }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_is_there() - exit  ---\n");
#endif

    return found;
}

static void
c2b_init_globals()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_globals() - enter ---\n");
#endif

    c2b_globals.input_format = NULL;
    c2b_globals.input_format_idx = UNDEFINED_FORMAT;
    c2b_globals.output_format = NULL;
    c2b_globals.output_format_idx = UNDEFINED_FORMAT;
    c2b_globals.all_reads_flag = kFalse;
    c2b_globals.keep_header_flag = kFalse;
    c2b_globals.split_flag = kFalse;
    c2b_globals.split_with_deletions_flag = kFalse;
    c2b_globals.zero_indexed_flag = kFalse;
    c2b_globals.reduced_flag = kFalse;
    c2b_globals.header_line_idx = 0U;
    c2b_globals.gff = NULL;
    c2b_init_global_gff_state();
    c2b_globals.gtf = NULL;
    c2b_init_global_gtf_state();
    c2b_globals.psl = NULL;
    c2b_init_global_psl_state();
    c2b_globals.rmsk = NULL; 
    c2b_init_global_rmsk_state();
    c2b_globals.sam = NULL;
    c2b_init_global_sam_state();
    c2b_globals.vcf = NULL;
    c2b_init_global_vcf_state(); 
    c2b_globals.wig = NULL;
    c2b_init_global_wig_state();
    c2b_globals.cat = NULL;
    c2b_init_global_cat_params();
    c2b_globals.sort = NULL;
    c2b_init_global_sort_params();
    c2b_globals.starch = NULL;
    c2b_init_global_starch_params();
    c2b_globals.src_line_str = NULL;
    c2b_globals.dest_line_str = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_globals() - exit  ---\n");
#endif
}

static void
c2b_delete_globals()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_globals() - enter ---\n");
#endif

    if (c2b_globals.output_format) free(c2b_globals.output_format), c2b_globals.output_format = NULL;
    if (c2b_globals.input_format) free(c2b_globals.input_format), c2b_globals.input_format = NULL;
    c2b_globals.input_format_idx = UNDEFINED_FORMAT;
    c2b_globals.all_reads_flag = kFalse;
    c2b_globals.keep_header_flag = kFalse;
    c2b_globals.split_flag = kFalse;
    c2b_globals.split_with_deletions_flag = kFalse;
    c2b_globals.reduced_flag = kFalse;
    c2b_globals.header_line_idx = 0U;
    if (c2b_globals.gff) c2b_delete_global_gff_state();
    if (c2b_globals.gtf) c2b_delete_global_gtf_state();
    if (c2b_globals.psl) c2b_delete_global_psl_state();
    if (c2b_globals.rmsk) c2b_delete_global_rmsk_state();
    if (c2b_globals.sam) c2b_delete_global_sam_state();
    if (c2b_globals.vcf) c2b_delete_global_vcf_state();
    if (c2b_globals.wig) c2b_delete_global_wig_state();
    if (c2b_globals.cat) c2b_delete_global_cat_params();
    if (c2b_globals.sort) c2b_delete_global_sort_params();
    if (c2b_globals.starch) c2b_delete_global_starch_params();
    if (c2b_globals.src_line_str) free(c2b_globals.src_line_str), c2b_globals.src_line_str = NULL;
    if (c2b_globals.dest_line_str) free(c2b_globals.dest_line_str), c2b_globals.dest_line_str = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_globals() - exit  ---\n");
#endif
}

static void
c2b_init_global_gff_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_gff_state() - enter ---\n");
#endif

    c2b_globals.gff = malloc(sizeof(c2b_gff_state_t));
    if (!c2b_globals.gff) {
        fprintf(stderr, "Error: Could not allocate space for GFF state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.gff->element = NULL, c2b_gff_init_element(&(c2b_globals.gff->element));

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_gff_state() - exit  ---\n");
#endif
}

static void             
c2b_delete_global_gff_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_gff_state() - enter ---\n");
#endif

    if (c2b_globals.gff->element) {
        c2b_gff_delete_element(c2b_globals.gff->element);
        c2b_globals.gff->element = NULL;
    }

    free(c2b_globals.gff), c2b_globals.gff = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_gff_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_gtf_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_gtf_state() - enter ---\n");
#endif

    c2b_globals.gtf = malloc(sizeof(c2b_gtf_state_t));
    if (!c2b_globals.gtf) {
        fprintf(stderr, "Error: Could not allocate space for GTF state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.gtf->element = NULL, c2b_gtf_init_element(&(c2b_globals.gtf->element));

    c2b_globals.gtf->line_count = 0;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_gtf_state() - exit  ---\n");
#endif
}

static void             
c2b_delete_global_gtf_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_gtf_state() - enter ---\n");
#endif

    if (c2b_globals.gtf->element) {
        c2b_gtf_delete_element(c2b_globals.gtf->element);
        c2b_globals.gtf->element = NULL;
    }

    free(c2b_globals.gtf), c2b_globals.gtf = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_gtf_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_psl_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_psl_state() - enter ---\n");
#endif

    c2b_globals.psl = malloc(sizeof(c2b_psl_state_t));
    if (!c2b_globals.psl) {
        fprintf(stderr, "Error: Could not allocate space for PSL state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.psl->element = NULL, c2b_psl_init_element(&(c2b_globals.psl->element));

    c2b_globals.psl->is_headered = kTrue;

    c2b_globals.psl->block = NULL;
    c2b_globals.psl->block = malloc(sizeof(c2b_psl_block_t));
    if (!c2b_globals.psl->block) {
        fprintf(stderr, "Error: Could not allocate space for PSL block state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.psl->block->max_count = 0;

    c2b_globals.psl->block->sizes = NULL;
    c2b_globals.psl->block->sizes = malloc(sizeof(uint64_t) * C2B_MAX_PSL_BLOCKS);
    if (!c2b_globals.psl->block->sizes) {
        fprintf(stderr, "Error: Could not allocate space for PSL block state sizes global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.psl->block->starts = NULL;
    c2b_globals.psl->block->starts = malloc(sizeof(uint64_t) * C2B_MAX_PSL_BLOCKS);
    if (!c2b_globals.psl->block->starts) {
        fprintf(stderr, "Error: Could not allocate space for PSL block state starts global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.psl->block->max_count = C2B_MAX_PSL_BLOCKS;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_psl_state() - exit  ---\n");
#endif
}

static void             
c2b_delete_global_psl_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_psl_state() - enter ---\n");
#endif

    if (c2b_globals.psl->element) {
        c2b_psl_delete_element(c2b_globals.psl->element);
        c2b_globals.psl->element = NULL;
    }
    free(c2b_globals.psl->block->starts), c2b_globals.psl->block->starts = NULL;
    free(c2b_globals.psl->block->sizes), c2b_globals.psl->block->sizes = NULL;
    c2b_globals.psl->block->max_count = 0;
    free(c2b_globals.psl->block), c2b_globals.psl->block = NULL;
    free(c2b_globals.psl), c2b_globals.psl = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_psl_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_rmsk_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_rmsk_state() - enter ---\n");
#endif

    c2b_globals.rmsk = malloc(sizeof(c2b_rmsk_state_t));
    if (!c2b_globals.rmsk) {
        fprintf(stderr, "Error: Could not allocate space for RepeatMasker annotation state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.rmsk->element = NULL, c2b_rmsk_init_element(&(c2b_globals.rmsk->element));
    c2b_globals.rmsk->line = 0U;
    c2b_globals.rmsk->is_start_of_line = kTrue;
    c2b_globals.rmsk->is_start_of_gap = kFalse;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_rmsk_state() - exit  ---\n");
#endif
}

static void             
c2b_delete_global_rmsk_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_rmsk_state() - enter ---\n");
#endif

    c2b_rmsk_delete_element(c2b_globals.rmsk->element);
    free(c2b_globals.rmsk), c2b_globals.rmsk = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_rmsk_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_sam_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_sam_state() - enter ---\n");
#endif

    c2b_globals.sam = malloc(sizeof(c2b_sam_state_t));
    if (!c2b_globals.sam) {
        fprintf(stderr, "Error: Could not allocate space for SAM state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.sam->samtools_path = NULL;

    c2b_globals.sam->cigar = NULL, c2b_sam_init_cigar_ops(&(c2b_globals.sam->cigar), C2B_SAM_CIGAR_OPS_VALUE_INITIAL);

    c2b_globals.sam->element = NULL, c2b_sam_init_element(&(c2b_globals.sam->element));

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_sam_state() - exit  ---\n");
#endif
}

static void
c2b_delete_global_sam_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_sam_state() - enter ---\n");
#endif

    if (c2b_globals.sam->samtools_path)
        free(c2b_globals.sam->samtools_path), c2b_globals.sam->samtools_path = NULL;
    
    if (c2b_globals.sam->cigar)
        c2b_sam_delete_cigar_ops(c2b_globals.sam->cigar);

    if (c2b_globals.sam->element)
        c2b_sam_delete_element(c2b_globals.sam->element);
    
    free(c2b_globals.sam), c2b_globals.sam = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_sam_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_vcf_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_vcf_state() - enter ---\n");
#endif
    
    c2b_globals.vcf = malloc(sizeof(c2b_vcf_state_t));
    if (!c2b_globals.vcf) {
        fprintf(stderr, "Error: Could not allocate space for VCF state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    
    c2b_globals.vcf->do_not_split = kFalse;
    c2b_globals.vcf->only_snvs = kFalse;
    c2b_globals.vcf->only_insertions = kFalse;
    c2b_globals.vcf->only_deletions = kFalse;    
    c2b_globals.vcf->filter_count = 0U;
    c2b_globals.vcf->element = NULL, c2b_vcf_init_element(&(c2b_globals.vcf->element));

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_vcf_state() - exit  ---\n");
#endif
}

static void
c2b_delete_global_vcf_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_vcf_state() - enter ---\n");
#endif

    if (c2b_globals.vcf->element) {
        c2b_vcf_delete_element(c2b_globals.vcf->element);
        c2b_globals.vcf->element = NULL;
    }

    free(c2b_globals.vcf), c2b_globals.vcf = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_vcf_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_wig_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_wig_state() - enter ---\n");
#endif

    c2b_globals.wig = malloc(sizeof(c2b_wig_state_t));
    if (!c2b_globals.wig) {
        fprintf(stderr, "Error: Could not allocate space for WIG state global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.wig->section = 1;
    c2b_globals.wig->line = 0;
    c2b_globals.wig->pos_lines = 0;
    c2b_globals.wig->span = 0;
    c2b_globals.wig->step = 0;
    c2b_globals.wig->start_pos = 0;
    c2b_globals.wig->end_pos = 0;
    c2b_globals.wig->score = 0.0f;
    c2b_globals.wig->chr = NULL;
    c2b_globals.wig->id = NULL;
    c2b_globals.wig->is_fixed_step = kFalse;
    c2b_globals.wig->start_write = kFalse;
    c2b_globals.wig->basename = NULL;
    c2b_globals.wig->start_shift = 1;
    c2b_globals.wig->end_shift = 1;

    c2b_globals.wig->chr = malloc(C2B_MAX_CHROMOSOME_LENGTH);
    if (!c2b_globals.wig->chr) {
        fprintf(stderr, "Error: Could not allocate space for global WIG chromosome string\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    memset(c2b_globals.wig->chr, 0, C2B_MAX_CHROMOSOME_LENGTH);

    c2b_globals.wig->id = malloc(C2B_MAX_FIELD_LENGTH_VALUE);
    if (!c2b_globals.wig->id) {
        fprintf(stderr, "Error: Could not allocate space for global WIG ID string\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    memset(c2b_globals.wig->id, 0, C2B_MAX_FIELD_LENGTH_VALUE);

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_wig_state() - exit  ---\n");
#endif
}

static void
c2b_delete_global_wig_state()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_wig_state() - enter ---\n");
#endif

    if (c2b_globals.wig->chr)
        free(c2b_globals.wig->chr), c2b_globals.wig->chr = NULL;

    if (c2b_globals.wig->id)
        free(c2b_globals.wig->id), c2b_globals.wig->id = NULL;

    if (c2b_globals.wig->basename)
        free(c2b_globals.wig->basename), c2b_globals.wig->basename = NULL;

    free(c2b_globals.wig), c2b_globals.wig = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_wig_state() - exit  ---\n");
#endif
}

static void
c2b_init_global_cat_params()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_cat_params() - enter ---\n");
#endif

    c2b_globals.cat = malloc(sizeof(c2b_cat_params_t));
    if (!c2b_globals.cat) {
        fprintf(stderr, "Error: Could not allocate space for cat parameters global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.cat->path = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_cat_params() - exit  ---\n");
#endif
}

static void
c2b_delete_global_cat_params()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_cat_params() - enter ---\n");
#endif

    if (c2b_globals.cat->path)
        free(c2b_globals.cat->path), c2b_globals.cat->path = NULL;

    free(c2b_globals.cat), c2b_globals.cat = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_cat_params() - exit  ---\n");
#endif
}

static void
c2b_init_global_sort_params()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_sort_params() - enter ---\n");
#endif

    c2b_globals.sort = malloc(sizeof(c2b_sort_params_t));
    if (!c2b_globals.sort) {
        fprintf(stderr, "Error: Could not allocate space for sort parameters global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.sort->is_enabled = kTrue;
    c2b_globals.sort->sort_bed_path = NULL;
    c2b_globals.sort->max_mem_value = NULL;
    c2b_globals.sort->sort_tmpdir_path = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_sort_params() - exit  ---\n");
#endif
}

static void
c2b_delete_global_sort_params()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_sort_params() - enter ---\n");
#endif

    if (c2b_globals.sort->max_mem_value)
        free(c2b_globals.sort->max_mem_value), c2b_globals.sort->max_mem_value = NULL;

    if (c2b_globals.sort->sort_bed_path)
        free(c2b_globals.sort->sort_bed_path), c2b_globals.sort->sort_bed_path = NULL;

    if (c2b_globals.sort->sort_tmpdir_path)
        free(c2b_globals.sort->sort_tmpdir_path), c2b_globals.sort->sort_tmpdir_path = NULL;

    free(c2b_globals.sort), c2b_globals.sort = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_sort_params() - exit  ---\n");
#endif
}

static void
c2b_init_global_starch_params()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_starch_params() - enter ---\n");
#endif

    c2b_globals.starch = malloc(sizeof(c2b_starch_params_t));
    if (!c2b_globals.starch) {
        fprintf(stderr, "Error: Could not allocate space for starch parameters global\n");
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }

    c2b_globals.starch->path = NULL;
    c2b_globals.starch->bzip2 = kFalse;
    c2b_globals.starch->gzip = kFalse;
    c2b_globals.starch->note = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_global_starch_params() - exit  ---\n");
#endif
}

static void
c2b_delete_global_starch_params()
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_starch_params() - enter ---\n");
#endif

    if (c2b_globals.starch->path)
        free(c2b_globals.starch->path), c2b_globals.starch->path = NULL;

    if (c2b_globals.starch->note)
        free(c2b_globals.starch->note), c2b_globals.starch->note = NULL;

    free(c2b_globals.starch), c2b_globals.starch = NULL;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_delete_global_starch_params() - exit  ---\n");
#endif
}

static void
c2b_init_command_line_options(int argc, char **argv)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_command_line_options() - enter ---\n");
#endif

    char *input_format = NULL;
    char *output_format = NULL;
    int client_long_index;
    int client_opt = getopt_long(argc,
                                 argv,
                                 c2b_client_opt_string,
                                 c2b_client_long_options,
                                 &client_long_index);

    opterr = 0; /* disable error reporting by GNU getopt */

    while (client_opt != -1) {
        switch (client_opt) 
            {
            case 'i':
                input_format = malloc(strlen(optarg) + 1);
                if (!input_format) {
                    fprintf(stderr, "Error: Could not allocate space for input format argument\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(input_format, optarg, strlen(optarg) + 1);
                c2b_globals.input_format = c2b_to_lowercase(input_format);
                c2b_globals.input_format_idx = c2b_to_input_format(c2b_globals.input_format);
                free(input_format), input_format = NULL;
                break;
            case 'o':
                output_format = malloc(strlen(optarg) + 1);
                if (!output_format) {
                    fprintf(stderr, "Error: Could not allocate space for output format argument\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(output_format, optarg, strlen(optarg) + 1);
                c2b_globals.output_format = c2b_to_lowercase(output_format);
                c2b_globals.output_format_idx = c2b_to_output_format(c2b_globals.output_format);
                free(output_format), output_format = NULL;
                break;
            case 'm':
                c2b_globals.sort->max_mem_value = malloc(strlen(optarg) + 1);
                if (!c2b_globals.sort->max_mem_value) {
                    fprintf(stderr, "Error: Could not allocate space for sort-bed max-mem argument\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.sort->max_mem_value, optarg, strlen(optarg) + 1);
                break;
            case 'r':
                c2b_globals.sort->sort_tmpdir_path = malloc(strlen(optarg) + 1);
                if (!c2b_globals.sort->sort_tmpdir_path) {
                    fprintf(stderr, "Error: Could not allocate space for sort-bed temporary directory argument\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.sort->sort_tmpdir_path, optarg, strlen(optarg) + 1);
                break;
            case 'e':
                c2b_globals.starch->note = malloc(strlen(optarg) + 1);
                if (!c2b_globals.starch->note) {
                    fprintf(stderr, "Error: Could not allocate space for Starch note\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.starch->note, optarg, strlen(optarg) + 1);
                break;
            case 'b':
                c2b_globals.wig->basename = malloc(strlen(optarg) + 1);
                if (!c2b_globals.wig->basename) {
                    fprintf(stderr, "Error: Could not allocate space for WIG basename\n");
                    c2b_print_usage(stderr);
                    exit(ENOMEM); /* Not enough space (POSIX.1) */
                }
                memcpy(c2b_globals.wig->basename, optarg, strlen(optarg) + 1);
                break;
            case 's':
                c2b_globals.split_flag = kTrue;
                c2b_globals.split_with_deletions_flag = kFalse;
                break;
            case 'S':
                c2b_globals.split_flag = kTrue;
                c2b_globals.split_with_deletions_flag = kTrue;
                break;
            case 'p':
                c2b_globals.vcf->do_not_split = kTrue;
                break;
            case 'R':
                c2b_globals.reduced_flag = kTrue;
                break;
            case 'v':
                c2b_globals.vcf->filter_count++;
                c2b_globals.vcf->only_snvs = kTrue;
                break;
            case 't':
                c2b_globals.vcf->filter_count++;
                c2b_globals.vcf->only_insertions = kTrue;
                break;
            case 'n':
                c2b_globals.vcf->filter_count++;
                c2b_globals.vcf->only_deletions = kTrue;
                break;
            case 'd':
                c2b_globals.sort->is_enabled = kFalse;
                break;
            case 'a':
                c2b_globals.all_reads_flag = kTrue;
                break;
            case 'k':
                c2b_globals.keep_header_flag = kTrue;
                break;
            case 'z':
                c2b_globals.starch->bzip2 = kTrue;
                break;
            case 'g':
                c2b_globals.starch->gzip = kTrue;
                break;
            case 'x':
                c2b_globals.wig->start_shift = 0;
                c2b_globals.wig->end_shift = 0;
                c2b_globals.zero_indexed_flag = kTrue;
                break;
            case 'h':
                c2b_print_usage(stdout);
                exit(EXIT_SUCCESS);
            case 'w':
                c2b_print_version(stdout);
                exit(EXIT_SUCCESS);
            case '1':
                c2b_globals.help_format_idx = BAM_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '2':
                c2b_globals.help_format_idx = GFF_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '3':
                c2b_globals.help_format_idx = GTF_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '4':
                c2b_globals.help_format_idx = GVF_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '5':
                c2b_globals.help_format_idx = PSL_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '6':
                c2b_globals.help_format_idx = RMSK_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '7':
                c2b_globals.help_format_idx = SAM_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '8':
                c2b_globals.help_format_idx = VCF_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '9':
                c2b_globals.help_format_idx = WIG_FORMAT;
                c2b_print_format_usage(stdout);
                exit(EXIT_SUCCESS);
            case '?':
                c2b_print_usage(stderr);
                exit(EXIT_SUCCESS);
            default:
                break;
        }
        client_opt = getopt_long(argc,
                                 argv,
                                 c2b_client_opt_string,
                                 c2b_client_long_options,
                                 &client_long_index);
    }

    if ((!c2b_globals.input_format) || (c2b_globals.input_format_idx == UNDEFINED_FORMAT)) {
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

    if ((!c2b_globals.output_format) || (c2b_globals.output_format_idx == UNDEFINED_FORMAT)) {
        c2b_globals.output_format = malloc(strlen(c2b_default_output_format) + 1);
        if (!c2b_globals.output_format) {
            fprintf(stderr, "Error: Could not allocate space for output format copy\n");
            c2b_print_usage(stderr);
            exit(ENOMEM); /* Not enough space (POSIX.1) */
        }
        memcpy(c2b_globals.output_format, c2b_default_output_format, strlen(c2b_default_output_format) + 1);
        c2b_globals.output_format_idx = c2b_to_output_format(c2b_globals.output_format);
    }

    if ((c2b_globals.input_format_idx == VCF_FORMAT) && (c2b_globals.vcf->filter_count > 1)) {
        fprintf(stderr, "Error: Cannot specify more than one VCF variant filter option\n");
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }
    
    if ((c2b_globals.starch->bzip2) && (c2b_globals.starch->gzip)) {
        fprintf(stderr, "Error: Cannot specify both Starch compression options\n");
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

    if (!(c2b_globals.starch->bzip2) && !(c2b_globals.starch->gzip) && (c2b_globals.output_format_idx == STARCH_FORMAT)) {
        c2b_globals.starch->bzip2 = kTrue;
    }
    else if ((c2b_globals.starch->bzip2 || c2b_globals.starch->gzip) && (c2b_globals.output_format_idx == BED_FORMAT)) {
        fprintf(stderr, "Error: Cannot specify Starch compression options without setting output format to Starch\n");
        c2b_print_usage(stderr);
        exit(EINVAL); /* Invalid argument (POSIX.1) */
    }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_init_command_line_options() - exit  ---\n");
#endif
}

static void
c2b_print_version(FILE* stream)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_version() - enter ---\n");
#endif

    fprintf(stream,
            "%s\n"                              \
            "  version:  %s\n"                  \
            "  author:   %s\n",
            application_name,
            application_version,
            application_authors);

#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_version() - exit  ---\n");
#endif
}

static void
c2b_print_usage(FILE* stream)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_usage() - enter ---\n");
#endif

    fprintf(stream,
            "%s\n"                  \
            "  version:  %s\n"      \
            "  author:   %s\n"      \
            "%s\n"                  \
            "%s\n"                  \
            "%s\n"                  \
            "%s\n",
            application_name,
            application_version,
            application_authors,
            general_usage,
            general_description,
            general_io_options, 
            general_options);

#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_usage() - exit  ---\n");
#endif
}

static void
c2b_print_format_usage(FILE* stream)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_format_usage() - enter ---\n");
#endif

    char *format_name = NULL;
    char *format_description = NULL;
    char *format_options = NULL;
    char *format_usage = NULL;

    switch(c2b_globals.help_format_idx) {
    case BAM_FORMAT:
        format_name = (char *) bam_name;
        format_usage = (char *) bam_usage;
        format_description = (char *) bam_description;
        format_options = (char *) bam_options;
        break;
    case GFF_FORMAT:
        format_name = (char *) gff_name;
        format_usage = (char *) gff_usage;
        format_description = (char *) gff_description;
        format_options = (char *) gff_options;
        break;
    case GTF_FORMAT:
        format_name = (char *) gtf_name;
        format_usage = (char *) gtf_usage;
        format_description = (char *) gtf_description;
        format_options = (char *) gtf_options;
        break;
    case GVF_FORMAT:
        format_name = (char *) gvf_name;
        format_usage = (char *) gvf_usage;
        format_description = (char *) gvf_description;
        format_options = (char *) gvf_options;
        break;
    case PSL_FORMAT:
        format_name = (char *) psl_name;
        format_usage = (char *) psl_usage;
        format_description = (char *) psl_description;
        format_options = (char *) psl_options;
        break;
    case RMSK_FORMAT:
        format_name = (char *) rmsk_name;
        format_usage = (char *) rmsk_usage;
        format_description = (char *) rmsk_description;
        format_options = (char *) rmsk_options;
        break;
    case SAM_FORMAT:
        format_name = (char *) sam_name;
        format_usage = (char *) sam_usage;
        format_description = (char *) sam_description;
        format_options = (char *) sam_options;
        break;
    case VCF_FORMAT:
        format_name = (char *) vcf_name;
        format_usage = (char *) vcf_usage;
        format_description = (char *) vcf_description;
        format_options = (char *) vcf_options;
        break;
    case WIG_FORMAT:
        format_name = (char *) wig_name;
        format_usage = (char *) wig_usage;
        format_description = (char *) wig_description;
        format_options = (char *) wig_options;
        break;
    default:
        format_name = (char *) application_name;
        format_usage = (char *) format_undefined_usage;
        format_description = (char *) general_description;
        format_options = (char *) general_options;
    }

    if (format_options) {
        fprintf(stream,
                "%s\n"                           \
                "  version:  %s\n"               \
                "  author:   %s\n\n"             \
                "%s\n"                           \
                "%s\n"                           \
                "%s\n"                           \
                "%s\n",
                format_name,
                application_version,
                application_authors,
                format_usage,
                format_description,
                format_options,
                general_options);
    }
    else {
        fprintf(stream,
                "%s\n"                           \
                "  version:  %s\n"               \
                "  author:   %s\n\n"             \
                "%s\n"                           \
                "%s\n"                           \
                "%s\n",
                format_name,
                application_version,
                application_authors,
                format_usage,
                format_description,
                general_options);
    }

#ifdef DEBUG
    fprintf(stderr, "--- c2b_print_format_usage() - exit  ---\n");
#endif
}

static char *
c2b_to_lowercase(const char* src)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_to_lowercase() - enter ---\n");
#endif

    char *dest = NULL;
    char *p = NULL;

    if (!src)
        return dest;

    p = malloc(strlen(src) + 1);
    if (!p) {
        fprintf(stderr, "Error: Could not allocate space for lowercase translation\n");
        c2b_print_usage(stderr);
        exit(ENOMEM); /* Not enough space (POSIX.1) */
    }
    memcpy(p, src, strlen(src) + 1);
    dest = p;
    for ( ; *p; ++p)
        *p = (*p >= 'A' && *p <= 'Z') ? (*p | 0x60) : *p;

#ifdef DEBUG
    fprintf(stderr, "--- c2b_to_lowercase() - exit  ---\n");
#endif
    return dest;
}

static c2b_format_t
c2b_to_input_format(const char* input_format)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_to_input_format() - enter ---\n");
    fprintf(stderr, "--- c2b_to_input_format() - exit  ---\n");
#endif

    return
        (strcmp(input_format, "bam") == 0)  ? BAM_FORMAT  :
        (strcmp(input_format, "gff") == 0)  ? GFF_FORMAT  :
        (strcmp(input_format, "gtf") == 0)  ? GTF_FORMAT  :
        (strcmp(input_format, "gvf") == 0)  ? GVF_FORMAT  :
        (strcmp(input_format, "psl") == 0)  ? PSL_FORMAT  :
        (strcmp(input_format, "rmsk") == 0) ? RMSK_FORMAT :
        (strcmp(input_format, "sam") == 0)  ? SAM_FORMAT  :
        (strcmp(input_format, "vcf") == 0)  ? VCF_FORMAT  :
        (strcmp(input_format, "wig") == 0)  ? WIG_FORMAT  :
        UNDEFINED_FORMAT;
}

static c2b_format_t
c2b_to_output_format(const char* output_format)
{
#ifdef DEBUG
    fprintf(stderr, "--- c2b_to_output_format() - enter ---\n");
    fprintf(stderr, "--- c2b_to_output_format() - exit  ---\n");
#endif

    return
        (strcmp(output_format, "bed") == 0) ? BED_FORMAT :
        (strcmp(output_format, "starch") == 0) ? STARCH_FORMAT :
        UNDEFINED_FORMAT;
}
