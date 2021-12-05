using System;
using System.ComponentModel.DataAnnotations;

namespace PHTRS.Models
{
    public class Pothole
    {
        public int Id { get; set; }
        public string Location { get; set; }
        public int Priority { get; set; }

        [Required]
        [Range (1,10)]
        public int Size { get; set; }

        [Required]
        [StringLength(60, MinimumLength = 3)]
        public string Street { get; set; }

        [Required]
        [StringLength(60, MinimumLength = 3)]
        public string District { get; set; }
        
        [Required]
        [DataType(DataType.Date)]
        public DateTime ReportDate { get; set; }

        public void editPriority()
        {
            if(this.Size>=4 && this.Size <= 7)
            {
                this.Priority = 2;
            }
            else if (this.Size > 7)
            {
                this.Priority = 1;
            }
            else
            {
                this.Priority = 3;
            }
        }
    }
}