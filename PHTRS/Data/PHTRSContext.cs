using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using PHTRS.Models;

namespace PHTRS.Data
{
    public class PHTRSContext : DbContext
    {
        public PHTRSContext (DbContextOptions<PHTRSContext> options)
            : base(options)
        {
        }

        public DbSet<PHTRS.Models.Pothole> Pothole { get; set; }
    }
}
